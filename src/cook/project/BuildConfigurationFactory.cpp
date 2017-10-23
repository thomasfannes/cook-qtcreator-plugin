#include "cook/project/BuildConfigurationFactory.hpp"
#include "cook/project/BuildConfiguration.hpp"
#include "cook/project/NinjaBuildStep.hpp"
#include "cook/project/CookNinjaStep.hpp"
#include "cook/project/Project.hpp"
#include "cook/Constants.hpp"
#include <projectexplorer/projectexplorerconstants.h>
#include <projectexplorer/buildsteplist.h>
#include <projectexplorer/target.h>
#include <projectexplorer/buildinfo.h>
#include <projectexplorer/project.h>
#include <utils/mimetypes/mimedatabase.h>
#include <utils/qtcassert.h>

namespace cook { namespace project {


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
    if (k && Utils::mimeTypeForFile(projectPath).matchesName(constants::COOK_MIME_TYPE))
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

    auto bc = new project::BuildConfiguration(parent, info->buildType);
    bc->setDisplayName(info->displayName);
    bc->setDefaultDisplayName(info->displayName);
    bc->setBuildDirectory(info->buildDirectory);

    ProjectExplorer::BuildStepList * buildSteps = bc->stepList(ProjectExplorer::Constants::BUILDSTEPS_BUILD);
    {
        // the cook step
        {
            Q_ASSERT(buildSteps);
            auto cookStep = new CookNinjaStep(buildSteps);
            buildSteps->insertStep(0, cookStep);
        }

        // the ninja step
        {
            Q_ASSERT(buildSteps);
            auto ninjaStep = new NinjaBuildStep(buildSteps);
            buildSteps->insertStep(1, ninjaStep);
        }
    }

    ProjectExplorer::BuildStepList * cleanSteps = bc->stepList(ProjectExplorer::Constants::BUILDSTEPS_CLEAN);
    {
        // the cook step
        {
            Q_ASSERT(cleanSteps);
            auto cookStep = new CookNinjaStep(cleanSteps);
            cleanSteps->insertStep(0, cookStep);
        }

        // the ninja step
        {
            Q_ASSERT(cleanSteps);
            auto ninjaStep = new NinjaBuildStep(cleanSteps);
            ninjaStep->add_build_target("clean", true);
            ninjaStep->set_clean(true);
            cleanSteps->insertStep(1, ninjaStep);
        }
    }

    return bc;
}

bool BuildConfigurationFactory::canClone(const ProjectExplorer::Target *parent, ProjectExplorer::BuildConfiguration *source) const
{
    if (!can_handle_(parent))
        return false;

    return source->id() == constants::COOK_BUILDCONFIG_ID;
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
    return ProjectExplorer::idFromMap(map) == constants::COOK_BUILDCONFIG_ID;
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

ProjectExplorer::BuildInfo * BuildConfigurationFactory::create_build_info_(const ProjectExplorer::Kit *k, const Utils::FileName &project_dir, ProjectExplorer::BuildConfiguration::BuildType build_type) const
{
    auto info = new ProjectExplorer::BuildInfo(this);

    info->buildDirectory = project_dir;
    info->buildDirectory.appendPath("build");

    info->kitId = k->id();
    info->buildType = build_type;


    switch(build_type)
    {
#define CASE(TYPE) case BuildConfiguration::TYPE: info->typeName = tr(#TYPE); break
    CASE(Debug);
    CASE(Unknown);
    CASE(Profile);
    CASE(Release);
#undef CASE
    }

    return info;
}

} }
