#include "cook/project/NinjaBuildStepFactory.hpp"
#include "cook/project/NinjaBuildStep.hpp"
#include "cook/project/Project.hpp"
#include "cook/Constants.hpp"
#include <projectexplorer/target.h>
#include <projectexplorer/buildsteplist.h>

namespace cook { namespace project {


NinjaBuildStepFactory::NinjaBuildStepFactory(QObject * parent)
    : ProjectExplorer::IBuildStepFactory(parent)
{
}

QList<ProjectExplorer::BuildStepInfo> NinjaBuildStepFactory::availableSteps(ProjectExplorer::BuildStepList */*parent*/) const
{
    return { ProjectExplorer::BuildStepInfo(constants::NINJA_BUILD_STEP_ID, tr("Ninja Build", "Display name for NinjaBuildStep id.")) };
}

ProjectExplorer::BuildStep * NinjaBuildStepFactory::create(ProjectExplorer::BuildStepList *parent, Core::Id id)
{
    Q_UNUSED(id)
    return new NinjaBuildStep(parent);
}

ProjectExplorer::BuildStep * NinjaBuildStepFactory::clone(ProjectExplorer::BuildStepList *parent, ProjectExplorer::BuildStep * source)
{
    return new NinjaBuildStep(parent, static_cast<NinjaBuildStep *>(source));
}


} }
