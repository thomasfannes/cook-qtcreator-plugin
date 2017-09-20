#include "qook/project/BuildConfigurationFactory.hpp"
#include "qook/project/BuildConfiguration.hpp"
#include "qook/project/BuildStep.hpp"
#include "qook/project/Project.hpp"
#include "qook/Constants.hpp"
#include <projectexplorer/projectexplorerconstants.h>
#include <projectexplorer/buildsteplist.h>
#include <projectexplorer/target.h>
#include <projectexplorer/buildinfo.h>
#include <projectexplorer/project.h>
#include <utils/mimetypes/mimedatabase.h>
#include <utils/qtcassert.h>

namespace qook { namespace project {


std::vector<ProjectExplorer::BuildConfiguration::BuildType> BuildConfigurationFactory::different_configurations_ = { ProjectExplorer::BuildConfiguration::Debug, ProjectExplorer::BuildConfiguration::Release };

BuildConfigurationFactory::BuildConfigurationFactory(QObject *parent)
    : ProjectExplorer::IBuildConfigurationFactory(parent)
{
}

BuildConfigurationFactory::~BuildConfigurationFactory()
{
}

int BuildConfigurationFactory::priority(const ProjectExplorer::Target *parent) const
{
    return can_handle_(parent) ? 0 : -1;
}

QList<ProjectExplorer::BuildInfo *> BuildConfigurationFactory::availableBuilds(const ProjectExplorer::Target *parent) const
{
    QList<ProjectExplorer::BuildInfo *> lst;

    for(BuildConfiguration::BuildType type : different_configurations_)
        lst << create_build_info_(parent->kit(), parent->project()->projectDirectory(), type);
    return lst;
}

int BuildConfigurationFactory::priority(const ProjectExplorer::Kit *k, const QString &projectPath) const
{
    if (k && Utils::mimeTypeForFile(projectPath).matchesName(constants::QOOK_MIME_TYPE))
        return 0;

    return -1;
}

QList<ProjectExplorer::BuildInfo *> BuildConfigurationFactory::availableSetups(const ProjectExplorer::Kit *k, const QString &projectPath) const
{
    QList<ProjectExplorer::BuildInfo *> result;

    for(BuildConfiguration::BuildType type : different_configurations_)
    {
        ProjectExplorer::BuildInfo * info = create_build_info_(k, Project::projectDirectory(Utils::FileName::fromString(projectPath)), type);
        if (type == BuildConfiguration::Unknown)
            info->displayName = tr("Default");
        else
            info->displayName = info->typeName;

        result << info;
    }
    return result;
}

ProjectExplorer::BuildConfiguration * BuildConfigurationFactory::create(ProjectExplorer::Target *parent, const ProjectExplorer::BuildInfo *info) const
{
    QTC_ASSERT(info->factory() == this, return 0);
    QTC_ASSERT(info->kitId == parent->kit()->id(), return 0);
    QTC_ASSERT(!info->displayName.isEmpty(), return 0);

    auto bc = new project::BuildConfiguration(parent);
    bc->setDisplayName(info->displayName);
    bc->setDefaultDisplayName(info->displayName);
    bc->setBuildDirectory(info->buildDirectory);

//    info->buildType;

    ProjectExplorer::BuildStepList * buildSteps = bc->stepList(ProjectExplorer::Constants::BUILDSTEPS_BUILD);
    {
        Q_ASSERT(buildSteps);
        auto makeStep = new CookBuildStep(buildSteps);
        buildSteps->insertStep(0, makeStep);
//        makeStep->setBuildTarget("all", /* on = */ true);

//        buildSteps
    }
    ProjectExplorer::BuildStepList * cleanSteps = bc->stepList(ProjectExplorer::Constants::BUILDSTEPS_CLEAN);

    Q_ASSERT(buildSteps);


    //    auto makeStep = new ProjectExplorer::GenericMakeStep(buildSteps);
    //    buildSteps->insertStep(0, makeStep);
    //    makeStep->setBuildTarget("all", /* on = */ true);

    Q_ASSERT(cleanSteps);
    //    auto cleanMakeStep = new GenericMakeStep(cleanSteps);
    //    cleanSteps->insertStep(0, cleanMakeStep);
    //    cleanMakeStep->setBuildTarget("clean", /* on = */ true);
    //    cleanMakeStep->setClean(true);

    return bc;
}

bool BuildConfigurationFactory::canClone(const ProjectExplorer::Target *parent, ProjectExplorer::BuildConfiguration *source) const
{
    if (!can_handle_(parent))
        return false;

    return source->id() == constants::QOOK_BUILDCONFIG_ID;
}

ProjectExplorer::BuildConfiguration * BuildConfigurationFactory::clone(ProjectExplorer::Target *parent, ProjectExplorer::BuildConfiguration *source)
{
    if (!canClone(parent, source))
        return 0;

    return new BuildConfiguration(parent, qobject_cast<BuildConfiguration *>(source));
}

bool BuildConfigurationFactory::canRestore(const ProjectExplorer::Target *parent, const QVariantMap &map) const
{
    if (!can_handle_(parent))
        return false;
    return ProjectExplorer::idFromMap(map) == constants::QOOK_BUILDCONFIG_ID;
}

ProjectExplorer::BuildConfiguration * BuildConfigurationFactory::restore(ProjectExplorer::Target *parent, const QVariantMap &map)
{
    if (!canRestore(parent, map))
        return 0;

    auto bc = new BuildConfiguration(parent);
    if (bc->fromMap(map))
        return bc;

    delete bc;
    return 0;
}

bool BuildConfigurationFactory::can_handle_(const ProjectExplorer::Target * target) const
{
    if (!target->project()->supportsKit(target->kit()))
        return false;

    return qobject_cast<Project *>(target->project());
}

ProjectExplorer::BuildInfo * BuildConfigurationFactory::create_build_info_(const ProjectExplorer::Kit *k, const Utils::FileName &buildDir, ProjectExplorer::BuildConfiguration::BuildType build_type) const
{
    auto info = new ProjectExplorer::BuildInfo(this);

    switch(build_type)
    {
#define CASE(TYPE) case BuildConfiguration::TYPE: info->typeName = tr(#TYPE); break
    CASE(Debug);
    CASE(Unknown);
    CASE(Profile);
    CASE(Release);
#undef CASE
    }

    info->buildDirectory = buildDir;
    info->kitId = k->id();
    info->buildType = build_type;

    return info;
}

} }
