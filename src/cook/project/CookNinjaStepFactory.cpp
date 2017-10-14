#include "cook/project/CookNinjaStepFactory.hpp"
#include "cook/project/CookNinjaStep.hpp"
#include "cook/project/Project.hpp"
#include "cook/Constants.hpp"
#include <projectexplorer/target.h>
#include <projectexplorer/buildsteplist.h>

namespace cook { namespace project {


CookNinjaStepFactory::CookNinjaStepFactory(QObject * parent)
    : ProjectExplorer::IBuildStepFactory(parent)
{
}

QList<ProjectExplorer::BuildStepInfo> CookNinjaStepFactory::availableSteps(ProjectExplorer::BuildStepList *parent) const
{
    if (parent->target()->project()->id() != constants::COOK_PROJECT_ID)
        return {};

    return { ProjectExplorer::BuildStepInfo(constants::COOK_BUILD_STEP_ID, tr("Cook Build", "Display name for CookBuildStep id.")) };
}

ProjectExplorer::BuildStep * CookNinjaStepFactory::create(ProjectExplorer::BuildStepList *parent, Core::Id id)
{
    Q_UNUSED(id)
    return new CookNinjaStep(parent);
}

ProjectExplorer::BuildStep * CookNinjaStepFactory::clone(ProjectExplorer::BuildStepList *parent, ProjectExplorer::BuildStep * source)
{
    return new CookNinjaStep(parent, static_cast<CookNinjaStep *>(source));
}


} }
