#include "cook/project/BuildConfiguration.hpp"
#include "cook/project/RunConfiguration.hpp"
#include "cook/project/BuildSettingsWidget.hpp"
#include "cook/project/InfoManager.hpp"
#include "cook/project/ProjectNodes.hpp"
#include "cook/project/Project.hpp"
#include "cook/toolset/KitInformation.hpp"
#include "cook/Constants.hpp"

#include <coreplugin/progressmanager/progressmanager.h>
#include <coreplugin/messagemanager.h>
#include <projectexplorer/target.h>
#include <projectexplorer/projectexplorerconstants.h>
#include <projectexplorer/buildinfo.h>
#include <utils/qtcassert.h>
#include <utils/algorithm.h>
#include <stack>

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

template <typename Functor>
void visit_tree(const info::Recipe & root, Functor && f)
{
    std::stack<const info::Recipe *> todo;
    todo.push(&root);

    while(!todo.empty())
    {
        const info::Recipe * cur = todo.top();
        todo.pop();

        if ( f(*cur) )
            for(const auto & p : cur->children)
                todo.push(&p);
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

Utils::FileName BuildConfiguration::project_file() const
{
    return project()->projectFilePath();
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

QStringList BuildConfiguration::ninja_build_args() const
{
    QStringList args;
    args << "-g" << "build.ninja";
    args << "-f" << project_file().toString();
    args << "-b" << buildDirectory().toString();
    args << "-n";

    switch(buildType())
    {
        case ProjectExplorer::BuildConfiguration::Debug:
            args << "-c" << "debug";
            break;

        case ProjectExplorer::BuildConfiguration::Release:
            args << "-c" << "release";
            break;

        default:
            break;
    }

    args << target_uri();
    return args;
}


const info::Recipes & BuildConfiguration::recipes_info_() const
{
    return info_mngr_->recipes().latest();
}

const info::Recipe & BuildConfiguration::root_book() const
{
    const auto & p = info_mngr_->recipes().latest();

    if (p.root.children.size() != 1)
        return p.root;
    else
        return p.root.children.front();
}

Utils::FileNameList BuildConfiguration::all_script_files() const
{
    Utils::FileNameList lst;

    visit_tree(recipes_info_().root, [&](const info::Recipe & recipe)
    {
        if(!recipe.script.isEmpty())
            lst << recipe.script;
        return true;
    });

    return lst;
}

QString BuildConfiguration::default_uri() const
{
    return recipes_info_().default_uri;
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

ProjectExplorer::ProjectNode * BuildConfiguration::generate_linear_project() const
{
    CookNode * root = new CookNode(project());

    using P = std::pair<const info::Recipe *, RecipeNode *>;
    std::stack<P> todo;
    todo.push(std::make_pair(&root_book(), nullptr));

    while (!todo.empty())
    {
        P p = todo.top();
        todo.pop();

        const info::Recipe & recipe = *p.first;

        RecipeNode * rn = new RecipeNode(recipe);
        for(const info::FileInfo & f : recipe.files)
        {
            auto * n = new ProjectExplorer::FileNode(f.file, convert(f.type), false);
            rn->addNestedNode(n);
        }

        ChaiScriptNode * cn = new ChaiScriptNode(recipe.script);
        rn->addNode(cn);
        rn->compress();

        // add to the parent level
        if (p.second == nullptr)
            root->addNode(rn);
        else
            p.second->addNode(rn);

        // and visit the children
        for(const auto & p : recipe.children)
            todo.push(std::make_pair(&p, rn));
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

    visit_tree(root_book(), [&](const info::Recipe & recipe) {

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

        return true;
    });

    const CppTools::ProjectUpdateInfo projectInfoUpdate(project(), cToolChain, cxxToolChain, k, rpps);
    cpp_updater->update(projectInfoUpdate);
}

const toolset::CookTool * BuildConfiguration::tool() const
{
    return toolset::KitInformation::cook_tool(target()->kit());
}

Project * BuildConfiguration::project() const
{
    return static_cast<Project *>(target()->project());
}

void BuildConfiguration::handle_request_started(InfoRequestType /*type*/)
{
    // before a request, make sure that the build directory exists
    if (request_.is_on_first())
    {
        clear_error_();
        project()->handle_parsing_started_(this, request_.flags());
    }
}

void BuildConfiguration::handle_request_finished(bool success, InfoRequestType type)
{
    Project * proj = project();

    // toggle the flag
    request_.set(type, success);
    proj->handle_sub_parsing_finished(this, type, success);

    // should we start a new one
    if (request_.is_finished())
    {
        proj->handle_parsing_finished_(this, request_.successes(), request_.failures());
    }
    else
    {
        InfoRequestType t = request_.propose_flag();
        start_refresh_(t);
    }

    if (success && type == InfoRequestType::Recipes)
        emit recipes_changed();
}

void BuildConfiguration::handle_error_occured(const QString & error, InfoRequestType /*type*/)
{
    set_error_(error);
}

void BuildConfiguration::handle_directory_changed()
{
    
}

void BuildConfiguration::ctor()
{
    connect(info_mngr_, &InfoManager::started, this, &BuildConfiguration::handle_request_started);
    connect(info_mngr_, &InfoManager::finished, this, &BuildConfiguration::handle_request_finished);
    connect(info_mngr_, &InfoManager::error_occurred, this, &BuildConfiguration::handle_error_occured);
    connect(this, &ProjectExplorer::BuildConfiguration::buildDirectoryChanged, this, &BuildConfiguration::handle_directory_changed);
}

const info::Recipe * BuildConfiguration::find_recipe(const QString & uri) const
{
    const info::Recipe * result = nullptr;

    visit_tree(root_book(), [&](const info::Recipe & recipe) {

        if(result)
            return false;

        if(recipe.uri == uri)
        {
            result = &recipe;
            return false;
        }

        return true;
    });

    return result;
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
                return recipes_info_().default_uri;
        }
        case Target_Default:
            return recipes_info_().default_uri;

        default:
            return QString();
    }
}


void BuildConfiguration::start_refresh_(InfoRequestType type)
{
    auto start_async_process = [this](auto & manager, const Core::Id & id)
    {
        QTC_ASSERT(manager.start_async(), return);
        Core::ProgressManager::addTask(manager.future(), "Cooking", id);
    };

    switch(type)
    {
        case InfoRequestType::Recipes:
        {
            start_async_process(info_mngr_->recipes(), "Cook.scan.recipes");
            connect(&info_mngr_->recipes(), &info::StructureManager::process_output, [](const QByteArray & array)
            {
                Core::MessageManager::write(QString::fromLatin1(array));
            });
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

void BuildConfiguration::set_build_target(const CookBuildTarget & target)
{
    if (target_ == target)
        return;

    target_ = target;
    emit target_uri_changed();
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
    tgts.append(all_run_targets());

    return tgts;
}


QList<CookBuildTarget> BuildConfiguration::all_run_targets() const
{
    QList<CookBuildTarget> tgts;

    visit_tree(root_book(), [&](const info::Recipe & recipe) {
        if (recipe.is_recipe)
            tgts << CookBuildTarget(recipe);
        return true;
    });

    return tgts;
}

QList<info::Element> BuildConfiguration::all_targets() const
{
    QList<info::Element> tgts;
    visit_tree(root_book(), [&](const info::Recipe & recipe) {
        if (recipe.is_recipe)
            tgts << recipe;
        return true;
    });

    return tgts;
}


} }


