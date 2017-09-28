#include "qook/project/CookBuildStep.hpp"
#include "qook/project/CookBuildStepConfigWidget.hpp"
#include "qook/project/BuildConfiguration.hpp"
#include "qook/project/Project.hpp"
#include "qook/toolset/Tool.hpp"
#include "qook/toolset/KitInformation.hpp"
#include "qook/Constants.hpp"
#include <projectexplorer/target.h>
#include <projectexplorer/projectexplorerconstants.h>
#include <projectexplorer/buildsteplist.h>
#include <utils/qtcassert.h>

namespace qook { namespace project {

namespace {

const char TARGET_NAME[] = "CookProjectManager.CookBuildStep.TargetName";
const char TARGET_URI[] = "CookProjectManager.CookBuildStep.TargetUri";
const char TARGET_EXECUTABLE[] = "CookProjectManager.CookBuildStep.TargetExecutable";
const char TARGET_SPECIAL[] = "CookProjectManager.CookBuildStep.TargetSpecial";
const char ADD_RUNCONFIGURATION_TEXT[] = "Current executable";

}

CookBuildStep::CookBuildStep(ProjectExplorer::BuildStepList *parent)
    :AbstractProcessStep(parent, constants::COOK_BUILD_STEP_ID)
{
    ctor_(parent);
}

CookBuildStep::CookBuildStep(ProjectExplorer::BuildStepList *parent, CookBuildStep *product)
    : AbstractProcessStep(parent, constants::COOK_BUILD_STEP_ID),
      build_target_(product->build_target_)
{
    ctor_(parent);
}

CookBuildStep::CookBuildStep(ProjectExplorer::BuildStepList * parent, Core::Id id)
    : AbstractProcessStep(parent, id)

{
    ctor_(parent);
}

void CookBuildStep::ctor_(ProjectExplorer::BuildStepList */*parent*/)
{
    if(build_target_.uri.isEmpty())
        set_build_target_(CookBuildTarget::current_executable());

    connect(project_(), &Project::recipes_available, this, &CookBuildStep::build_targets_changed);
}

Project * CookBuildStep::project_() const
{
    return static_cast<Project *>(project());
}

bool CookBuildStep::fromMap(const QVariantMap &map)
{
    build_target_.display_name = map.value(TARGET_NAME).toString();
    build_target_.uri = map.value(TARGET_URI).toString();
    build_target_.executable = Utils::FileName::fromString(map.value(TARGET_EXECUTABLE).toString());
    build_target_.is_special = map.value(TARGET_SPECIAL).toBool();

    return AbstractProcessStep::fromMap(map);
}

QVariantMap CookBuildStep::toMap() const
{
    QVariantMap map(AbstractProcessStep::toMap());

    map.insert(TARGET_NAME, build_target_.display_name);
    map.insert(TARGET_URI, build_target_.uri);
    map.insert(TARGET_EXECUTABLE, build_target_.executable.toString());
    map.insert(TARGET_SPECIAL, build_target_.is_special);

    return map;
}


bool CookBuildStep::init(QList<const BuildStep *> &earlierSteps)
{
    ProjectExplorer::ProcessParameters * pp = processParameters();
    if (!configure_process_parameters_(*pp, run_configuration_(), true))
        return false;

    pp->resolveAll();

    return AbstractProcessStep::init(earlierSteps);
}

void CookBuildStep::run(QFutureInterface<bool> & fi)
{
    AbstractProcessStep::run(fi);
}


ProjectExplorer::BuildStepConfigWidget * CookBuildStep::createConfigWidget()
{
    return new CookBuildStepConfigWidget(this);
}

QList<CookBuildTarget> CookBuildStep::all_targets() const
{
    return build_configuration_()->all_targets();
}

bool CookBuildStep::builds_target(const CookBuildTarget & target) const
{
    return target == build_target_;
}

BuildConfiguration * CookBuildStep::build_configuration_() const
{
    return static_cast<BuildConfiguration*>(buildConfiguration());
}

RunConfiguration * CookBuildStep::run_configuration_() const
{
    return nullptr;
}

void CookBuildStep::set_build_target_(const CookBuildTarget & target)
{
    if (build_target_ == target)
        return;

    build_target_ = target;
    emit build_target_changed();
}

QString CookBuildStep::all_arguments_(RunConfiguration *run_configuration) const
{
    QString arguments;

    Utils::QtcProcess::addArg(&arguments, "--generate");
    Utils::QtcProcess::addArg(&arguments, "build.ninja");

    Utils::QtcProcess::addArg(&arguments, "--input");
    Utils::QtcProcess::addArg(&arguments, project()->projectFilePath().toString());

    Utils::QtcProcess::addArg(&arguments, "--build-dir");
    Utils::QtcProcess::addArg(&arguments, ".");

    switch(buildConfiguration()->buildType())
    {
    case ProjectExplorer::BuildConfiguration::Debug:
        Utils::QtcProcess::addArg(&arguments, "--config");
        Utils::QtcProcess::addArg(&arguments, "debug");
        break;

        case ProjectExplorer::BuildConfiguration::Release:
        Utils::QtcProcess::addArg(&arguments, "--config");
        Utils::QtcProcess::addArg(&arguments, "release");
        break;

    default:
        break;
    }

    /*if(build_target_ == CookBuildTarget::all())
    {
        Utils::QtcProcess::addArg(&arguments, "--target-all");
    }
    else if(build_target_ == CookBuildTarget::clean())
    {
        Utils::QtcProcess::addArg(&arguments, "clean");
    }
    else */if(build_target_.is_special)
    {
        QString target =  "<i>&lt;" + build_target_.display_name+ "&gt;</i>";
        Utils::QtcProcess::addArg(&arguments, target);
    }
    else
    {
        Utils::QtcProcess::addArg(&arguments, build_target_.uri);
    }

    return arguments;
}

bool CookBuildStep::configure_process_parameters_(ProjectExplorer::ProcessParameters & param, RunConfiguration * run_configuration, bool needs_init)
{
    bool can_init = true;

    // find the configuiration
    BuildConfiguration * bc = static_cast<BuildConfiguration *>(build_configuration_());
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

    const toolset::Tool * tool = bc->tool();
    if(!tool)
    {
        emit addTask(ProjectExplorer::Task(ProjectExplorer::Task::Error, tr("A Cook tool must be set up for building. Configure a Cook tool in the kit options."), Utils::FileName(), -1, ProjectExplorer::Constants::TASK_CATEGORY_BUILDSYSTEM));
        can_init = false;
    }

    if(build_target_ == CookBuildTarget::current_executable() && !run_configuration)
    {
        emit addTask(ProjectExplorer::Task(ProjectExplorer::Task::Error, tr("You asked to build the current Run Configuration's build target only, but it is not associated with a build target."),  Utils::FileName(), -1, ProjectExplorer::Constants::TASK_CATEGORY_BUILDSYSTEM));
        can_init = false;
    }

    if(can_init || !needs_init)
    {
        param.setMacroExpander(bc->macroExpander());
        param.setEnvironment(bc->environment());
        param.setWorkingDirectory(bc->buildDirectory().toString());
        param.setCommand(bc->tool()->exec_file().filePath());
        param.setArguments(all_arguments_(run_configuration));
    }

    return can_init;
}


} }
