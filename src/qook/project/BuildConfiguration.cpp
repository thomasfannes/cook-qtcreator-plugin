#include "qook/project/BuildConfiguration.hpp"
#include "qook/project/BuildSettingsWidget.hpp"
#include "qook/project/CookScriptManager.hpp"
#include "qook/project/Project.hpp"
#include "qook/toolset/KitInformation.hpp"
#include "qook/Constants.hpp"

#include <coreplugin/progressmanager/progressmanager.h>
#include <projectexplorer/target.h>
#include <projectexplorer/buildinfo.h>
#include <utils/qtcassert.h>

namespace qook { namespace project {




BuildConfiguration::BuildConfiguration(ProjectExplorer::Target * parent)
    : ProjectExplorer::BuildConfiguration(parent, constants::QOOK_BUILDCONFIG_ID),
      type_(BuildConfiguration::Unknown),
      mngr_(new CookScriptManager(this))
{
    ctor();
}


BuildConfiguration::BuildConfiguration(ProjectExplorer::Target *parent, BuildConfiguration * source)
    : ProjectExplorer::BuildConfiguration(parent, source),
      type_(source->type_),
      mngr_(new CookScriptManager(this))
{
    cloneSteps(source);
    ctor();
}

BuildConfiguration::~BuildConfiguration()
{
    if(mngr_->future().isFinished())
    {
        mngr_->future().cancel();
        mngr_->future().waitForFinished();
        mngr_->deleteLater();
    }
}

QStringList BuildConfiguration::tree_format_options() const
{
    return  {"-t", "-s", project()->projectDirectory().toString()};
}

const toolset::Tool * BuildConfiguration::tool() const
{
    return toolset::KitInformation::tool(target()->kit());
}

void BuildConfiguration::refresh()
{
    mngr_->start_async_parse();
    Core::ProgressManager::addTask(mngr_->future(), "Cooking", "Cook.scan.recipes");
}

Project * BuildConfiguration::project_() const
{
    return static_cast<Project *>(project());
}

const Cook & BuildConfiguration::cook_info() const
{
    return mngr_->last();
}

void BuildConfiguration::ctor()
{
    Project * project = project_();
    connect(mngr_, &CookScriptManager::error_occured, this,
            [this, project](const QString & error)
    {
        set_error_(error);
        project->handle_parsing_error_(this);
    });

    connect(mngr_, &CookScriptManager::data_available, this,
            [this, project]()
    {
        clear_error_();
        project->update_project_data_(this, mngr_->last());
    });

    connect(mngr_, &CookScriptManager::started, this,
            [this, project]()
    {
        clear_error_();
       project->handle_parsing_started_(this);
    });
}



void BuildConfiguration::set_error_(const QString & error)
{
    error_ = error;

    emit error_occured(error_);
}

void BuildConfiguration::clear_error_()
{
    error_.clear();
}

ProjectExplorer::NamedWidget * BuildConfiguration::createConfigWidget()
{
    return new BuildSettingsWidget(this);
}

void BuildConfiguration::set_build_type(BuildType type)
{
    type_ = type;
}

BuildConfiguration::BuildType BuildConfiguration::buildType() const
{
    return type_;
}

QList<CookBuildTarget> BuildConfiguration::special_targets() const
{
    QList<CookBuildTarget> tgts;
    tgts.append(CookBuildTarget::all());
    tgts.append(CookBuildTarget::current_executable());
    tgts.append(CookBuildTarget::clean());

    return tgts;
}

QList<CookBuildTarget> BuildConfiguration::registered_targets() const
{
    QList<CookBuildTarget> tgts;
    for(const Recipe & recipe: cook_info().recipes)
    {
        QString name = recipe.uri;
        if(!recipe.display_name.isEmpty())
            name.append(QString(" (%1)").arg(recipe.display_name));

        tgts << CookBuildTarget(name, recipe.uri, buildDirectory());
    }

    return tgts;
}

QList<CookBuildTarget> BuildConfiguration::all_targets() const
{
    return special_targets() + registered_targets();
}


} }
