#include "qook/project/BuildConfiguration.hpp"
#include "qook/project/BuildSettingsWidget.hpp"
#include "qook/project/InfoManager.hpp"
#include "qook/project/ProjectNodes.hpp"
#include "qook/project/Project.hpp"
#include "qook/toolset/KitInformation.hpp"
#include "qook/Constants.hpp"

#include <coreplugin/progressmanager/progressmanager.h>
#include <projectexplorer/target.h>
#include <projectexplorer/projectexplorerconstants.h>
#include <projectexplorer/buildinfo.h>
#include <utils/qtcassert.h>
#include <utils/algorithm.h>

namespace qook { namespace project {

namespace  {

ProjectExplorer::FileType convert(info::FileType ft)
{
    switch(ft)
    {
        case info::FileType::Header:
            return ProjectExplorer::FileType::Header;

        case info::FileType::Source:
            return ProjectExplorer::FileType::Source;

        default:
            return ProjectExplorer::FileType::Unknown;
    }
}

}

BuildConfiguration::BuildConfiguration(ProjectExplorer::Target * parent)
    : ProjectExplorer::BuildConfiguration(parent, constants::QOOK_BUILDCONFIG_ID),
      type_(BuildConfiguration::Unknown),
      target_(CookBuildTarget::default_target()),
      info_mngr_(new InfoManager(this))
{
    ctor();
}

BuildConfiguration::BuildConfiguration(ProjectExplorer::Target *parent, BuildConfiguration * source)
    : ProjectExplorer::BuildConfiguration(parent, source),
      type_(source->type_),
      target_(CookBuildTarget::default_target()),
      info_mngr_(new InfoManager(this))
{
    cloneSteps(source);
    ctor();
}

BuildConfiguration::~BuildConfiguration()
{
    delete info_mngr_;
}

QStringList BuildConfiguration::all_recipes_options() const
{
    return {"-g", "recipes.tree", "-f", project()->projectFilePath().toString(), "-b", buildDirectory().toString() };
}

QStringList BuildConfiguration::recipe_detail_options(const QString & uri) const
{
    return {"-g", "details.tree", "-f", project()->projectFilePath().toString(), "-b", buildDirectory().toString(), uri };
}

QStringList BuildConfiguration::build_options(const QString & uri) const
{
    return {"-g", "build.ninja", "-f", project()->projectFilePath().toString(), uri };
}

const info::Recipes & BuildConfiguration::recipes_info() const
{
    return info_mngr_->recipes().latest();
}
const info::BuildRecipes &BuildConfiguration::build_recipes_info() const
{
    return info_mngr_->build_recipes().latest();
}

QString BuildConfiguration::target_uri() const
{
    if (target_ == CookBuildTarget::default_target())
    {
        return recipes_info().default_uri;
    }
    else if (target_.is_special)
        return QString();

    return target_.uri;
}

bool BuildConfiguration::refresh(QFlags<InfoRequestType> flags)
{
    if (!request_.is_finished())
        return false;

    request_ = ParseRequestRun(flags);
    // empty request
    if(request_.is_finished())
        return false;

    start_refresh_(request_.propose_flag());
    return true;
}

ProjectExplorer::ProjectNode * BuildConfiguration::generate_tree() const
{
    CookNode * root = new CookNode(project_());

    for (const info::BuildRecipe & r : build_recipes_info().recipes)
    {
        RecipeNode * rn = new RecipeNode(r);

        for(const info::FileInfo & f : r.files)
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

void BuildConfiguration::refresh_cpp_code_model(CppTools::CppProjectUpdater * cpp_updater)
{
    const ProjectExplorer::Kit * k = target()->kit();
    QTC_ASSERT(k, return);

    ProjectExplorer::ToolChain * cToolChain = ProjectExplorer::ToolChainKitInformation::toolChain(k, ProjectExplorer::Constants::C_LANGUAGE_ID);
    ProjectExplorer::ToolChain * cxxToolChain = ProjectExplorer::ToolChainKitInformation::toolChain(k, ProjectExplorer::Constants::CXX_LANGUAGE_ID);

    cpp_updater->cancel();

    CppTools::RawProjectParts rpps;

    for(const info::BuildRecipe & recipe : build_recipes_info().recipes)
    {
        CppTools::RawProjectPart rpp;
        rpp.setDisplayName(recipe.uri);
        rpp.setProjectFileLocation(recipe.script.toString());
        rpp.setIncludePaths(recipe.include_paths);

        QStringList files;
        for(const info::FileInfo & f :recipe.files)
            if(f.type == info::FileType::Header || f.type == info::FileType::Source)
                files.append(f.file.toString());

        rpp.setFiles(files);

        rpps.append(rpp);
    }

    const CppTools::ProjectUpdateInfo projectInfoUpdate(project(), cToolChain, cxxToolChain, k, rpps);
    cpp_updater->update(projectInfoUpdate);
}

const toolset::CookTool * BuildConfiguration::tool() const
{
    return toolset::KitInformation::cook_tool(target()->kit());
}

Project * BuildConfiguration::project_() const
{
    return static_cast<Project *>(project());
}

void BuildConfiguration::handle_request_started(InfoRequestType /*type*/)
{
    if (!request_.is_started())
    {
        clear_error_();
        project_()->handle_parsing_started_(this, request_.flags());
    }
}

void BuildConfiguration::handle_request_finished(bool success, InfoRequestType type)
{
    auto * project = project_();

    // toggle the flag
    request_.set(type, success);
    project->handle_sub_parsing_finished(this, type, success);

    // should we start a new one
    if (request_.is_finished())
    {
        project->handle_parsing_finished_(this, request_.successes(), request_.failures());
    }
    else
    {
        InfoRequestType t = request_.propose_flag();
        start_refresh_(t);
    }
}
void BuildConfiguration::handle_error_occured(const QString & error, InfoRequestType /*type*/)
{
    set_error_(error);
}

void BuildConfiguration::ctor()
{
    connect(info_mngr_, &InfoManager::started, this, &BuildConfiguration::handle_request_started);
    connect(info_mngr_, &InfoManager::finished, this, &BuildConfiguration::handle_request_finished);
    connect(info_mngr_, &InfoManager::error_occurred, this, &BuildConfiguration::handle_error_occured);
}

const info::Recipe * BuildConfiguration::find_recipe(const QString & uri) const
{
    auto it = recipes_info().recipes.find(uri);
    return (it == recipes_info().recipes.end() ? nullptr : &*it);
}

const info::BuildRecipe * BuildConfiguration::find_build_recipe(const QString & uri) const
{
    auto it = build_recipes_info().recipes.find(uri);
    return (it == build_recipes_info().recipes.end() ? nullptr : &*it);
}

void BuildConfiguration::start_refresh_(InfoRequestType type)
{
    switch(type)
    {
        case InfoRequestType::Recipes:
        {
            QTC_ASSERT(info_mngr_->recipes().start_async(), return);
            Core::ProgressManager::addTask(info_mngr_->recipes().future(), "Cooking", "Cook.scan.recipes");
            break;
        }

        case InfoRequestType::Build_Recipes:
        {
            QString uri = target_uri();
            if (!find_recipe(uri))
            {
                handle_request_started(type);
                handle_error_occured(QString("Unknown uri: <%1>").arg(uri), type);
                handle_request_finished(false, type);
            }
            else
            {
                QTC_ASSERT(info_mngr_->build_recipes().start_async(target_uri()), return);
                Core::ProgressManager::addTask(info_mngr_->build_recipes().future(), "Cooking", "Cook.scan.recipes");
            }
            break;
        }

        default:
            bool unknown_info_request_type = false;
            QTC_ASSERT(unknown_info_request_type, return);
    }
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

BuildConfiguration::BuildType BuildConfiguration::buildType() const
{
    return type_;
}

QList<CookBuildTarget> BuildConfiguration::special_targets_() const
{
    QList<CookBuildTarget> tgts;
    tgts.append(CookBuildTarget::current_executable());

    return tgts;
}

QList<CookBuildTarget> BuildConfiguration::all_build_targets() const
{    
    QList<CookBuildTarget> tgts = special_targets_();

    for(const info::Recipe & recipe: recipes_info().recipes)
        tgts << CookBuildTarget(recipe);

    return tgts;
}

QList<CookBuildTarget> BuildConfiguration::all_run_targets() const
{
    QList<CookBuildTarget> tgts;

    for(const info::BuildRecipe & recipe: build_recipes_info().recipes)
        tgts << CookBuildTarget(recipe);

    return tgts;
}


} }
