#include "qook/project/BuildConfiguration.hpp"
#include "qook/project/BuildSettingsWidget.hpp"
#include "qook/project/CookScriptManager.hpp"
#include "qook/project/Project.hpp"
#include "qook/project/ProjectNodes.hpp"
#include "qook/toolset/KitInformation.hpp"
#include "qook/Constants.hpp"

#include <coreplugin/progressmanager/progressmanager.h>
#include <projectexplorer/target.h>
#include <projectexplorer/buildinfo.h>
#include <utils/qtcassert.h>

namespace qook { namespace project {

namespace  {

ProjectExplorer::FileType convert(FileType ft)
{
    switch(ft)
    {
    case FileType::Header:
        return ProjectExplorer::FileType::Header;

    case FileType::Source:
        return ProjectExplorer::FileType::Source;

    default:
        return ProjectExplorer::FileType::Unknown;
    }
}

}


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

ProjectExplorer::ProjectNode * BuildConfiguration::generate_project_tree() const
{
    CookNode * root = new CookNode(project_());

    for (const Recipe & r : cook_info().recipes)
    {
        RecipeNode * rn = new RecipeNode(r);

        for(const FileInfo & f : r.files)
        {
                auto * n = new ProjectExplorer::FileNode(f.file, convert(f.type), false);
                rn->addNestedNode(n);
        }

        ChaiScriptNode * cn = new ChaiScriptNode(r.script);
        rn->addNode(cn);

        rn->compress();
        root->addNode(rn);
    }


    return root;
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
        project->update_project_data_(this);
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



} }
