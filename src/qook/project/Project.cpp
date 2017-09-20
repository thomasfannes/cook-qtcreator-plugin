#include "qook/project/Project.hpp"
#include "qook/project/BuildConfiguration.hpp"
#include "qook/Constants.hpp"
#include "qook/toolset/KitInformation.hpp"
#include <projectexplorer/projectexplorerconstants.h>
#include <projectexplorer/target.h>
#include <coreplugin/icontext.h>
#include <utils/qtcassert.h>
#include <QDebug>


namespace qook { namespace project {

Project::Project(const Utils::FileName & filename)
    : ProjectExplorer::Project(constants::QOOK_MIME_TYPE, filename, [this]() { refresh(); }),
      connected_target_(nullptr),
      current_build_config_(nullptr),
      cpp_code_model_updater_(new CppTools::CppProjectUpdater(this))
{
    setId(constants::QOOK_PROJECT_ID);
    setProjectLanguages(Core::Context(ProjectExplorer::Constants::CXX_LANGUAGE_ID));
    setDisplayName(filename.toFileInfo().baseName());

    connect(this, &ProjectExplorer::Project::activeTargetChanged, this, &Project::active_target_changed);
}

Project::~Project()
{
    delete cpp_code_model_updater_;
}

void Project::active_target_changed(ProjectExplorer::Target *target)
{
    if (connected_target_)
    {
        disconnect(connected_target_, &ProjectExplorer::Target::activeBuildConfigurationChanged, this, &Project::refresh);
        disconnect(connected_target_, &ProjectExplorer::Target::kitChanged, this, &Project::refresh);
    }

    connected_target_ = target;

    if (connected_target_)
    {
        connect(connected_target_, &ProjectExplorer::Target::activeBuildConfigurationChanged, this, &Project::refresh);
        connect(connected_target_, &ProjectExplorer::Target::kitChanged, this, &Project::refresh);
    }

    refresh();
}

void Project::refresh()
{
    current_build_config_ = static_cast<BuildConfiguration*>(connected_target_->activeBuildConfiguration());
    QTC_ASSERT(current_build_config_, return);
    current_build_config_->refresh();
}

void Project::handle_parsing_started_(BuildConfiguration * configuration)
{
    auto * t = activeTarget();
    if(!t || t->activeBuildConfiguration() != configuration)
        return;

    emitParsingStarted();
}

void Project::handle_parsing_error_(BuildConfiguration * configuration)
{
    // only emit if this is the active build configuration
    auto * t = activeTarget();
    if (!t || t->activeBuildConfiguration() != configuration)
        return;

    emitParsingFinished(false);
}

void Project::update_project_data_(BuildConfiguration * configuration)
{
    // make sure we are updating the active built configuration
    auto * t = activeTarget();
    if(!t || t->activeBuildConfiguration() != configuration)
        return;

    auto * root = configuration->generate_project_tree();
    setRootProjectNode(root);

    refresh_cpp_code_model_();

    emitParsingFinished(true);
}

bool Project::supportsKit(ProjectExplorer::Kit *k, QString *errorMessage) const
{
    if (!toolset::KitInformation::tool(k))
    {
        if(errorMessage)
            *errorMessage = tr("No Cook tool set.");
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

void Project::refresh_cpp_code_model_()
{
    QTC_ASSERT(current_build_config_, return);
    const ProjectExplorer::Kit * k = activeTarget()->kit();
    QTC_ASSERT(k, return);

    ProjectExplorer::ToolChain * cToolChain = ProjectExplorer::ToolChainKitInformation::toolChain(k, ProjectExplorer::Constants::C_LANGUAGE_ID);
    ProjectExplorer::ToolChain * cxxToolChain = ProjectExplorer::ToolChainKitInformation::toolChain(k, ProjectExplorer::Constants::CXX_LANGUAGE_ID);

    cpp_code_model_updater_->cancel();

    CppTools::RawProjectParts rpps;

    for(const Recipe & recipe : current_build_config_->cook_info().recipes)
    {
        CppTools::RawProjectPart rpp;
        rpp.setDisplayName(recipe.uri);
        rpp.setProjectFileLocation(recipe.script.toString());
        rpp.setIncludePaths(recipe.include_paths);

        QStringList files;
        for(const FileInfo & f :recipe.files)
            if(f.type == FileType::Header || f.type == FileType::Source)
                files.append(f.file.toString());

        rpp.setFiles(files);

        rpps.append(rpp);
    }

    const CppTools::ProjectUpdateInfo projectInfoUpdate(this, cToolChain, cxxToolChain, k, rpps);
    cpp_code_model_updater_->update(projectInfoUpdate);
}


} }
