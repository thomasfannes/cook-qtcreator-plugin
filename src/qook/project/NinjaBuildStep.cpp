#include "qook/project/NinjaBuildStep.hpp"
#include "qook/project/NinjaBuildStepConfigWidget.hpp"
#include "qook/Constants.hpp"

#include <projectexplorer/target.h>
#include <projectexplorer/project.h>
#include <projectexplorer/projectexplorerconstants.h>
#include <projectexplorer/task.h>
#include <projectexplorer/buildconfiguration.h>
#include <projectexplorer/gnumakeparser.h>
#include <projectexplorer/kit.h>
#include <projectexplorer/toolchain.h>
#include <projectexplorer/kitinformation.h>

namespace qook { namespace project {

NinjaBuildStep::NinjaBuildStep(ProjectExplorer::BuildStepList *parent)
    : ProjectExplorer::AbstractProcessStep(parent, constants::NINJA_BUILD_STEP_ID)
{
    ctor_();
}

NinjaBuildStep::NinjaBuildStep(ProjectExplorer::BuildStepList * parent, Core::Id id)
    : ProjectExplorer::AbstractProcessStep(parent, id)
{
    ctor_();
}

NinjaBuildStep::NinjaBuildStep(ProjectExplorer::BuildStepList *parent, NinjaBuildStep * product)
    : ProjectExplorer::AbstractProcessStep(parent, constants::NINJA_BUILD_STEP_ID),
      additional_arguments_(product->additional_arguments_)
{
    ctor_();
}
void NinjaBuildStep::ctor_()
{
    percent_progress_ = QRegExp("^\\[\\s*(\\d*)%\\]");
    ninja_progress_ = QRegExp("^\\[\\s*(\\d*)/\\s*(\\d*)");
    ninja_progress_string_ = "[%f/%t "; // ninja: [33/100
}

bool NinjaBuildStep::init(QList<const BuildStep *> &earlierSteps)
{
    ProjectExplorer::ProcessParameters * pp = processParameters();
    if (!configure_process_parameters_(*pp, true))
        return false;

    pp->resolveAll();

    setOutputParser(new ProjectExplorer::GnuMakeParser);
    ProjectExplorer::IOutputParser *parser = target()->kit()->createOutputParser();
    if (parser)
        appendOutputParser(parser);
    outputParser()->setWorkingDirectory(pp->effectiveWorkingDirectory());

    return AbstractProcessStep::init(earlierSteps);
}

void NinjaBuildStep::run(QFutureInterface<bool> &fi)
{
    AbstractProcessStep::run(fi);
}

bool NinjaBuildStep::fromMap(const QVariantMap &map)
{
    return AbstractProcessStep::fromMap(map);
}

QVariantMap NinjaBuildStep::toMap() const
{
    return AbstractProcessStep::toMap();
}

ProjectExplorer::BuildStepConfigWidget * NinjaBuildStep::createConfigWidget()
{
    return new NinjaBuildStepConfigWidget(this);
}

void NinjaBuildStep::stdOutput(const QString &line)
{
    if (ninja_progress_.indexIn(line) != -1)
    {
        AbstractProcessStep::stdOutput(line);

        bool ok = false;
        int done = ninja_progress_.cap(1).toInt(&ok);
        if (ok)
        {
            int all = ninja_progress_.cap(2).toInt(&ok);
            if (ok && all != 0)
            {
                futureInterface()->setProgressRange(0, 100);
                const int percent = static_cast<int>(100.0 * done/all);
                futureInterface()->setProgressValue(percent);
            }
        }
    }
    else
        AbstractProcessStep::stdError(line);
}

bool NinjaBuildStep::configure_process_parameters_(ProjectExplorer::ProcessParameters & param, bool needs_init)
{
    bool can_init = true;

    // find the configuiration
    auto * bc = buildConfiguration();
    if(!bc)
        bc = target()->activeBuildConfiguration();
    if(!bc)
    {
        emit addTask(ProjectExplorer::Task::buildConfigurationMissingTask());
        can_init = false;
    }

    if(bc && !bc->isEnabled())
    {
        emit addTask(ProjectExplorer::Task(ProjectExplorer::Task::Error, tr("The build configuration is currently disabled."), Utils::FileName(), -1, ProjectExplorer::Constants::TASK_CATEGORY_BUILDSYSTEM));
        can_init = false;
    }

    if(can_init || !needs_init)
    {
        param.setMacroExpander(bc->macroExpander());

        // set the environment
        {
            Utils::Environment env = bc->environment();
            Utils::Environment::setupEnglishOutput(&env);
            if (!env.value("NINJA_STATUS").startsWith(ninja_progress_string_))
                env.set("NINJA_STATUS", ninja_progress_string_ + "%o/sec] ");

            param.setEnvironment(bc->environment());
        }

        param.setWorkingDirectory(project()->projectDirectory().toString());
        param.setCommand("/usr/bin/ninja");

        {
            QString arguments = additional_arguments_;

            Utils::QtcProcess::addArg(&arguments, "-f");
            Utils::QtcProcess::addArg(&arguments, bc->buildDirectory().appendPath("build.ninja").toString());

            param.setArguments(arguments);
        }

    }

    return can_init;
}



} }
