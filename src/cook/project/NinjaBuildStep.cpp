#include "cook/project/NinjaBuildStep.hpp"
#include "cook/project/NinjaBuildStepConfigWidget.hpp"
#include "cook/toolset/KitInformation.hpp"
#include "cook/toolset/NinjaTool.hpp"
#include "cook/Constants.hpp"
#include <projectexplorer/target.h>
#include <projectexplorer/project.h>
#include <projectexplorer/projectexplorerconstants.h>
#include <projectexplorer/task.h>
#include <projectexplorer/buildconfiguration.h>
#include <projectexplorer/gnumakeparser.h>
#include <projectexplorer/kit.h>
#include <projectexplorer/toolchain.h>
#include <projectexplorer/kitinformation.h>
#include <projectexplorer/processparameters.h>
#include <QVariantMap>

namespace cook { namespace project {

namespace  {

const char BUILD_TARGETS_KEY[] = "CookProjectManager.Build.Ninja.Targets";
const char ADDITIONAL_ARGUMENTS_KEY[] = "CookProjectManager.Build.Ninja.AdditionalArguments";
const char CLEAN_KEY[] = "CookProjectManager.Build.Ninja.Clean";

}


NinjaBuildStep::NinjaSettings::NinjaSettings()
    : progress("^\\[\\s*(\\d*)/\\s*(\\d*)"),
      progress_string("[%f/%t ")
{
}

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

NinjaBuildStep::NinjaBuildStep(ProjectExplorer::BuildStepList *parent, NinjaBuildStep * bs)
    : ProjectExplorer::AbstractProcessStep(parent, constants::NINJA_BUILD_STEP_ID),
      build_targets_(bs->build_targets_),
      additional_arguments_(bs->additional_arguments()),
      clean_(bs->clean_)
{
    ctor_();
}
void NinjaBuildStep::ctor_()
{
    setDefaultDisplayName(tr("Ninja"));
//    percent_progress_ = QRegExp("^\\[\\s*(\\d*)%\\]");
//    ninja_progress_ = QRegExp("^\\[\\s*(\\d*)/\\s*(\\d*)");
//    ninja_progress_string_ = "[%f/%t "; // ninja: [33/100
}

bool NinjaBuildStep::init(QList<const BuildStep *> &earlierSteps)
{

    setIgnoreReturnValue(clean_);

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
    fi.setProgressRange(0, 100);
    AbstractProcessStep::run(fi);
}

ProjectExplorer::BuildStepConfigWidget * NinjaBuildStep::createConfigWidget()
{
    return new NinjaBuildStepConfigWidget(this);
}

bool NinjaBuildStep::immutable() const
{
    return false;
}

void NinjaBuildStep::add_build_target(const QString &target, bool on)
{
    QStringList old = build_targets_;
    if (on && !old.contains(target))
         old << target;
    else if (!on && old.contains(target))
        old.removeOne(target);

    build_targets_ = old;
}

void NinjaBuildStep::set_additional_arguments(const QString &list)
{
    if (list == additional_arguments_)
        return;

    additional_arguments_ = list;

    emit additional_arguments_changed(list);
}

QString NinjaBuildStep::additional_arguments() const
{
    return additional_arguments_;
}

QVariantMap NinjaBuildStep::toMap() const
{
    QVariantMap map = AbstractProcessStep::toMap();

    map.insert(BUILD_TARGETS_KEY, build_targets_);
    map.insert(ADDITIONAL_ARGUMENTS_KEY, additional_arguments_);
    map.insert(CLEAN_KEY, clean_);
    return map;
}

bool NinjaBuildStep::fromMap(const QVariantMap &map)
{
    build_targets_ = map.value(BUILD_TARGETS_KEY).toStringList();
    additional_arguments_ = map.value(ADDITIONAL_ARGUMENTS_KEY).toString();
    clean_ = map.value(CLEAN_KEY).toBool();

    return BuildStep::fromMap(map);
}

void NinjaBuildStep::stdOutput(const QString &line)
{
    if (ninja_settings_.progress.indexIn(line) != -1)
    {
        AbstractProcessStep::stdOutput(line);

        bool ok = false;
        int done = ninja_settings_.progress.cap(1).toInt(&ok);
        if (ok)
        {
            int all = ninja_settings_.progress.cap(2).toInt(&ok);
            if (ok && all != 0)
            {
                const int percent = static_cast<int>(100.0 * done/all);
                futureInterface()->setProgressValue(percent);
            }
        }
    }
    else
        AbstractProcessStep::stdError(line);
}

QString NinjaBuildStep::process_arguments_() const
{
    QString arguments;

    Utils::QtcProcess::addArg(&arguments, "-f");
    Utils::QtcProcess::addArg(&arguments, QString("%1/build.ninja").arg(buildConfiguration()->buildDirectory().toString()));

    Utils::QtcProcess::addArgs(&arguments, additional_arguments());
    Utils::QtcProcess::addArgs(&arguments, build_targets_);

    return arguments;
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

    const toolset::NinjaTool * tool = target() ? toolset::KitInformation::ninja_tool(target()->kit()) : nullptr;
    if(!tool)
    {
        emit addTask(ProjectExplorer::Task(ProjectExplorer::Task::Error, tr("A Ninja tool must be set up for building. Configure a Ninja tool in the kit options."), Utils::FileName(), -1, ProjectExplorer::Constants::TASK_CATEGORY_BUILDSYSTEM));
        can_init = false;
    }

    if(can_init || !needs_init)
    {
        param.setMacroExpander(bc->macroExpander());

        // set the environment
        {
            Utils::Environment env = bc->environment();
            Utils::Environment::setupEnglishOutput(&env);
            if (!env.value("NINJA_STATUS").startsWith(ninja_settings_.progress_string))
                env.set("NINJA_STATUS", ninja_settings_.progress_string + "%o/sec] ");

            param.setEnvironment(bc->environment());
        }

        param.setWorkingDirectory(project()->projectDirectory().toString());
        param.setCommand(tool->exec_file().absoluteFilePath());
        param.setArguments(process_arguments_());
    }

    return can_init;
}


} }
