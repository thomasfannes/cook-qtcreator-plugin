#include "cook/project/BuildConfiguration.hpp"
#include "cook/project/RunConfiguration.hpp"
#include "cook/project/BuildSettingsWidget.hpp"
#include "cook/project/InfoManager.hpp"
#include "cook/project/ProjectNodes.hpp"
#include "cook/project/Project.hpp"
#include "cook/toolset/KitInformation.hpp"
#include "cook/Constants.hpp"

#include <coreplugin/progressmanager/progressmanager.h>
#include <projectexplorer/target.h>
#include <projectexplorer/projectexplorerconstants.h>
#include <projectexplorer/buildinfo.h>
#include <utils/qtcassert.h>
#include <utils/algorithm.h>

namespace cook { namespace project {

namespace  {

const char BUILD_TYPE[] = "CookProject.BuildConfiguration.BuildType";

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

BuildConfiguration::BuildConfiguration(ProjectExplorer::Target * parent, const BuildType &build_type)
    : ProjectExplorer::BuildConfiguration(parent, constants::COOK_BUILDCONFIG_ID),
      type_(build_type),
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

QVariantMap BuildConfiguration::toMap() const
{
    QVariantMap map = ProjectExplorer::BuildConfiguration::toMap();
    map.insert(BUILD_TYPE, static_cast<int>(type_));

    return map;
}

bool BuildConfiguration::fromMap(const QVariantMap &map)
{
    type_ = static_cast<BuildType>(map.value(BUILD_TYPE, BuildType::Unknown).toInt());
    return ProjectExplorer::BuildConfiguration::fromMap(map);
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
    return {"-g", "build.ninja", "-f", project()->projectFilePath().toString(), "-b", buildDirectory().toString(), uri };
}

const info::Recipes & BuildConfiguration::recipes_info() const
{
    return info_mngr_->recipes().latest();
}
const info::BuildRecipes &BuildConfiguration::build_recipes_info() const
{
    return info_mngr_->build_recipes().latest();
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
    if (request_.is_on_first())
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

    if (success && type == InfoRequestType::Recipes)
        emit build_targets_changed();;
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
    connect(this, &BuildConfiguration::build_target_changed, [this]() { refresh(InfoRequestType::Build_Recipes | InfoRequestType::Ninja); });
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


QString BuildConfiguration::target_uri() const
{
    switch(target_.type)
    {
        case Target_URI:
            return target_.uri;

        case Target_CurrentExecutable:
        {
            RunConfiguration * rc = (target() ? qobject_cast<RunConfiguration*>(target()->activeRunConfiguration()) : nullptr);
            if (rc)
                return rc->target().uri;
            else
                return recipes_info().default_uri;
        }
        case Target_Default:
            return recipes_info().default_uri;

        default:
            return QString();
    }
}

bool BuildConfiguration::is_valid_uri(const QString & uri) const
{
    return !uri.isNull() && find_recipe(uri);
}

void BuildConfiguration::start_refresh_(InfoRequestType type)
{
    auto start_async_process = [this](auto & manager, const Core::Id & id)
    {
        QTC_ASSERT(manager.start_async(), return);
        Core::ProgressManager::addTask(manager.future(), "Cooking", id);
    };

    auto handle_faulty_uri = [&]()
    {
        QString uri = target_uri();
        if (!is_valid_uri(uri))
        {
            handle_request_started(type);
            handle_error_occured(QString("Unknown uri: <%1>").arg(uri), type);
            handle_request_finished(false, type);
            return false;
        }

        return true;
    };


    switch(type)
    {
        case InfoRequestType::Recipes:
            start_async_process(info_mngr_->recipes(), "Cook.scan.recipes");
            break;

        case InfoRequestType::Build_Recipes:
            if(handle_faulty_uri())
                start_async_process(info_mngr_->build_recipes(), "Cook.scan.detailed_recipes");
            break;

        case InfoRequestType::Ninja:
            if(handle_faulty_uri())
                start_async_process(info_mngr_->ninja_build(), "Cook.build.ninja");
            break;

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

void BuildConfiguration::set_build_target(const CookBuildTarget & target)
{
    if (target_ == target)
        return;

    target_ = target;
    emit build_target_changed();
}

const CookBuildTarget & BuildConfiguration::build_target() const
{
    return target_;
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
    tgts << CookBuildTarget::default_target();
    tgts << CookBuildTarget::current_executable();

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


