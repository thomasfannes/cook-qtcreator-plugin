#include "cook/project/Project.hpp"
#include "cook/project/ProjectNodes.hpp"
#include "cook/project/BuildConfiguration.hpp"
#include "cook/Constants.hpp"
#include "cook/toolset/KitInformation.hpp"
#include <projectexplorer/projectexplorerconstants.h>
#include <projectexplorer/target.h>
#include <projectexplorer/taskhub.h>
#include <coreplugin/icontext.h>
#include <utils/qtcassert.h>
#include <QDebug>


namespace cook { namespace project {

Project::Project(const Utils::FileName & filename)
    : ProjectExplorer::Project(constants::COOK_MIME_TYPE, filename, [this]() { refresh_all(); }),
      connected_target_(nullptr),
      current_build_config_(nullptr),
      cpp_code_model_updater_(new CppTools::CppProjectUpdater(this))
{
    setId(constants::COOK_PROJECT_ID);
    setProjectLanguages(Core::Context(ProjectExplorer::Constants::CXX_LANGUAGE_ID));
    setProjectContext(Core::Context(constants::COOK_PROJECT_CONTEXT));
    setDisplayName(filename.toFileInfo().baseName());

    connect(this, &ProjectExplorer::Project::activeTargetChanged, this, &Project::active_target_changed);
}

Project::~Project()
{
    delete cpp_code_model_updater_;
}

void Project::active_target_changed(ProjectExplorer::Target *target)
{
    BuildConfiguration * build_config = target ? static_cast<BuildConfiguration*>(target->activeBuildConfiguration()) : nullptr;

    if (connected_target_)
    {
        disconnect(connected_target_, &ProjectExplorer::Target::activeBuildConfigurationChanged, this, &Project::refresh_all);
        disconnect(connected_target_, &ProjectExplorer::Target::kitChanged, this, &Project::refresh_all);
    }

    if (current_build_config_)
    {
        disconnect(current_build_config_, &BuildConfiguration::recipes_changed, this, &Project::recipes_changed);
        disconnect(current_build_config_, &BuildConfiguration::target_uri_changed, this, &Project::target_uri_changed);
    }



    connected_target_ = target;
    current_build_config_ = build_config;

    if (connected_target_)
    {
        connect(connected_target_, &ProjectExplorer::Target::activeBuildConfigurationChanged, this, &Project::refresh_all);
        connect(connected_target_, &ProjectExplorer::Target::kitChanged, this, &Project::refresh_all);
    }

    if (current_build_config_)
    {
        connect(current_build_config_, &BuildConfiguration::recipes_changed, this, &Project::recipes_changed);
        connect(current_build_config_, &BuildConfiguration::target_uri_changed, this, &Project::target_uri_changed);
    }

    refresh_all();
}

BuildConfiguration * Project::active_build_configuration() const
{
    return current_build_config_;
}

void Project::refresh_all()
{
    refresh_(InfoRequestType::Recipes);
}

void Project::refresh_(RequestFlags flags)
{
    QTC_ASSERT(active_build_configuration(), return);
    active_build_configuration()->refresh(flags);
}

void Project::handle_parsing_started_(BuildConfiguration * configuration, RequestFlags /*flags*/)
{
    ProjectExplorer::TaskHub::clearTasks();

    auto * t = activeTarget();
    if(!t || t->activeBuildConfiguration() != configuration)
        return;
}

void Project::handle_parsing_finished_(BuildConfiguration * configuration, RequestFlags /*succeeded*/, RequestFlags /*failed*/)
{
    auto * t = activeTarget();
    if(!t || t->activeBuildConfiguration() != configuration)
        return;
}

void Project::handle_sub_parsing_finished(BuildConfiguration * configuration, InfoRequestType request, bool success)
{
    // only emit if this is the active build configuration
    auto * t = activeTarget();
    if (!t || t->activeBuildConfiguration() != configuration)
        return;

    if (request == InfoRequestType::Recipes)
    {
        if (success)
            handle_build_recipes_available_();
        else
            handle_parsing_error_();
    }
    if(!success)
        ProjectExplorer::TaskHub::addTask(ProjectExplorer::Task::Error, configuration->error(), ProjectExplorer::Constants::TASK_CATEGORY_BUILDSYSTEM);
}

bool Project::supportsKit(ProjectExplorer::Kit *k, QString *errorMessage) const
{
    if (!toolset::KitInformation::cook_tool(k))
    {
        if(errorMessage)
            *errorMessage = tr("No Cook tool set.");
        return false;
    }

    if (!toolset::KitInformation::ninja_tool(k))
    {
        if(errorMessage)
            *errorMessage = tr("No Ninja tool set.");
        return false;
    }

    return true;
}

bool Project::needsConfiguration() const
{
    return targets().isEmpty();
}


ProjectExplorer::Project::RestoreResult Project::fromMap(const QVariantMap &map, QString *errorMessage)
{
    return ProjectExplorer::Project::fromMap(map, errorMessage);
}

bool Project::setupTarget(ProjectExplorer::Target *t)
{
    t->updateDefaultBuildConfigurations();
    if (t->buildConfigurations().isEmpty())
        return false;
    t->updateDefaultDeployConfigurations();

    return true;
}

void Project::handle_parsing_error_()
{
    BuildConfiguration * bc = active_build_configuration();
    for(const info::Error & error : bc->all_parse_errors())
    {
        ProjectExplorer::TaskHub::addTask(
                    ProjectExplorer::Task::Error,
                    error.error,
                    ProjectExplorer::Constants::TASK_CATEGORY_BUILDSYSTEM,
                    error.script,
                    error.line
                    );
    }
}

void Project::handle_build_recipes_available_()
{
    activeTarget()->updateDefaultRunConfigurations();

    BuildConfiguration * bc = active_build_configuration();

    QTC_ASSERT(bc, return);
    auto root = bc->generate_linear_project();
    setRootProjectNode(root);
    bc->refresh_cpp_code_model(cpp_code_model_updater_);
    setDisplayName(rootProjectNode()->displayName());

    const QString & main_uri = bc->default_uri();
    const info::Recipe * active_recipe = bc->find_recipe(main_uri);
    QTC_ASSERT(active_recipe, return);

    // add watcher for all the script filenames
    {
        qDeleteAll(project_documents);
        project_documents.clear();
        for(const Utils::FileName & script_fn : current_build_config_->all_script_files())
            project_documents << new ProjectExplorer::ProjectDocument(constants::COOK_MIME_TYPE, script_fn, [this] { refresh_all(); });
    }
}


} }
