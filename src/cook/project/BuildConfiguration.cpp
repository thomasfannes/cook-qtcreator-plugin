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
#include <projectexplorer/taskhub.h>
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
      target_(CookBuildTarget::current_executable()),
      info_mngr_(new InfoManager(this))
{
    ctor();
}

BuildConfiguration::BuildConfiguration(ProjectExplorer::Target *parent, BuildConfiguration * source)
    : ProjectExplorer::BuildConfiguration(parent, source),
      type_(source->type_),
      target_(CookBuildTarget::current_executable()),
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

QStringList BuildConfiguration::ninja_build_args(const QStringList & additional_arguments) const
{
    QStringList args;

    args << "-g" << "build.ninja";
    args << "-f" << project_file().toString();
    args << "-o" << buildDirectory().toString();


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
    args.append(additional_arguments);

    args << "-n";
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
    return p.root;
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

QList<info::Error> BuildConfiguration::all_parse_errors() const
{
    return recipes_info_().errors;
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

namespace  {

CookNode::Type find_node_type(CookNode * root, const info::Recipe & recipe)
{
    if (root == nullptr)
        return CookNode::Type_RootNode;
    else if (recipe.is_recipe)
        return CookNode::Type_RecipeNode;
    else
        return CookNode::Type_BookNode;
}

}

ProjectExplorer::ProjectNode * BuildConfiguration::generate_linear_project() const
{
    CookNode * root = nullptr;

    using P = std::pair<const info::Recipe *, CookNode *>;
    std::stack<P> todo;
    todo.push(std::make_pair(&root_book(), nullptr));

    QSet<Utils::FileName> recipes;

    while (!todo.empty())
    {
        P p = todo.top();
        todo.pop();

        const info::Recipe & recipe = *p.first;
        CookNode * rn = new CookNode(recipe, find_node_type(root, recipe));
        for(const info::FileInfo & f : recipe.files)
        {
            auto * n = new ProjectExplorer::FileNode(f.file, convert(f.type), false);
            rn->addNestedNode(n);
        }

        // add all the recipes
        recipes.unite(recipe.all_scripts);
        if (!recipe.script.isEmpty())
            recipes.insert(recipe.script);

        rn->compress();

        // add to the parent level
        if (p.second == nullptr)
            root = rn;
        else
            p.second->addNode(rn);

        // and visit the children
        for(const auto & p : recipe.children)
            todo.push(std::make_pair(&p, rn));
    }


    {
        RecipesNode * recipes_folder = new RecipesNode(project()->projectDirectory());

        for(const auto & fn : recipes)
        {
            ChaiScriptNode * cn = new ChaiScriptNode(fn);
            recipes_folder->addNestedNode(cn);
        }

        recipes_folder->setDisplayName("recipes");

        root->addNode(recipes_folder);
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

        {
            QStringList files;
            for(const info::FileInfo & f :recipe.files)
                if(f.type == info::FileType::Header || f.type == info::FileType::Source)
                    files.append(f.file.toString());
            rpp.setFiles(files);
        }

        {
            ProjectExplorer::Macros macros;
            for(auto it = recipe.defines.begin(); it != recipe.defines.end(); ++it)
                if (it.value().isNull())
                    macros.append(ProjectExplorer::Macro(it.key().toLatin1()));
                else
                    macros.append(ProjectExplorer::Macro(it.key().toLatin1(), it.value().toLatin1()));

            rpp.setMacros(macros);
        }


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

Project * BuildConfiguration::project_() const
{
    return static_cast<Project *>(target()->project());
}

void BuildConfiguration::handle_request_started()
{
    // before a request, make sure that the build directory exists
    if (request_.is_on_first())
    {
        clear_error_();
        project_()->handle_parsing_started_(this, request_.flags());
    }
}

void BuildConfiguration::handle_request_finished(bool success, InfoRequestType type)
{
    Project * proj = project_();

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
    ProjectExplorer::TaskHub::addTask(ProjectExplorer::Task::Error, error, ProjectExplorer::Constants::TASK_CATEGORY_BUILDSYSTEM);
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

    connect(info_mngr_, &InfoManager::started,
            [](const QString & command) {
        Core::MessageManager::write(QString("Executing %1").arg(command));
    }
    );

    connect(info_mngr_, &InfoManager::process_output, [](const QString & output) { Core::MessageManager::write(output); });
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
            start_async_process(info_mngr_->recipes(), "Cook.scan.recipes");
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


