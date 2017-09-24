#include "qook/project/CookBuildStepFactory.hpp"
#include "qook/project/CookBuildStep.hpp"
#include "qook/project/Project.hpp"
#include "qook/Constants.hpp"
#include <projectexplorer/target.h>
#include <projectexplorer/buildsteplist.h>

namespace qook { namespace project {


CookBuildStepFactory::CookBuildStepFactory(QObject * parent)
    : ProjectExplorer::IBuildStepFactory(parent)
{
}

QList<ProjectExplorer::BuildStepInfo> CookBuildStepFactory::availableSteps(ProjectExplorer::BuildStepList *parent) const
{
    if (parent->target()->project()->id() != constants::QOOK_PROJECT_ID)
        return {};

    return { ProjectExplorer::BuildStepInfo(constants::COOK_BUILD_STEP_ID, tr("Cook Build", "Display name for CookProjectManager::CookBuilStep id.")) };
}

ProjectExplorer::BuildStep * CookBuildStepFactory::create(ProjectExplorer::BuildStepList *parent, Core::Id id)
{
    Q_UNUSED(id)
    return new CookBuildStep(parent);
}

ProjectExplorer::BuildStep * CookBuildStepFactory::clone(ProjectExplorer::BuildStepList *parent, ProjectExplorer::BuildStep * source)
{
    return new CookBuildStep(parent, static_cast<CookBuildStep *>(source));
}


} }
