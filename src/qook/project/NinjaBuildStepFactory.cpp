#include "qook/project/NinjaBuildStepFactory.hpp"
#include "qook/project/NinjaBuildStep.hpp"
#include "qook/project/Project.hpp"
#include "qook/Constants.hpp"
#include <projectexplorer/target.h>
#include <projectexplorer/buildsteplist.h>

namespace qook { namespace project {


NinjaBuildStepFactory::NinjaBuildStepFactory(QObject * parent)
    : ProjectExplorer::IBuildStepFactory(parent)
{
}

QList<ProjectExplorer::BuildStepInfo> NinjaBuildStepFactory::availableSteps(ProjectExplorer::BuildStepList *parent) const
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
