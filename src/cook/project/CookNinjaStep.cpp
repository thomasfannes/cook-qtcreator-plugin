#include "cook/project/CookNinjaStep.hpp"
#include "cook/project/CookNinjaStepConfigWidget.hpp"
#include "cook/project/BuildConfiguration.hpp"
#include "cook/toolset/CookTool.hpp"
#include "cook/Constants.hpp"
#include <projectexplorer/target.h>
#include <projectexplorer/projectexplorerconstants.h>

namespace cook { namespace project {

namespace {

const char ADDITIONAL_ARGUMENTS_KEY[] = "CookProjectManager.Build.Cook.AdditionalArguments";
const char CLEAN_KEY[] = "CookProjectManager.Build.Cook.Clean";

}

CookNinjaStep::CookNinjaStep(ProjectExplorer::BuildStepList *bsl)
    : ProjectExplorer::AbstractProcessStep(bsl, constants::COOK_BUILD_STEP_ID),
      clean_(false)
{
    ctor_();
}

CookNinjaStep::CookNinjaStep(ProjectExplorer::BuildStepList *bsl, CookNinjaStep *bs)
    : ProjectExplorer::AbstractProcessStep(bsl, bs),
      clean_(bs->clean_)
{
    ctor_();
}

CookNinjaStep::CookNinjaStep(ProjectExplorer::BuildStepList *bsl, Core::Id id)
    : ProjectExplorer::AbstractProcessStep(bsl, id),
      clean_(false)
{
    ctor_();
}

bool CookNinjaStep::get_process_parameters_(ProjectExplorer::ProcessParameters & pp)
{
    // get the build configuration
    BuildConfiguration *bc = qobject_cast<BuildConfiguration *>(buildConfiguration());
    if (!bc)
        bc = qobject_cast<BuildConfiguration *>(target()->activeBuildConfiguration());
    if (!bc)
        emit addTask(ProjectExplorer::Task::buildConfigurationMissingTask());

    if (!bc)
    {
        emitFaultyConfigurationMessage();
        return false;
    }

    // get the tool
    const toolset::CookTool * tool = bc->tool();
    if (!tool || !tool->is_valid())
    {
        ProjectExplorer::Task task(ProjectExplorer::Task::Error, QCoreApplication::translate("ProjectExplorer::Task", "No valid Cook tool specified. Configure the cook tool in the kit."), Utils::FileName(), -1, ProjectExplorer::Constants::TASK_CATEGORY_BUILDSYSTEM);
        emit addTask(task);
        return false;
    }

    pp.setMacroExpander(bc->macroExpander());
    pp.setWorkingDirectory(project()->projectDirectory().toString());
    Utils::Environment env = bc->environment();
    Utils::Environment::setupEnglishOutput(&env);
    pp.setEnvironment(env);
    pp.setCommand(tool->exec_file().absoluteFilePath());
    {
        QString args;

        Utils::QtcProcess::addArgs(&args, bc->ninja_build_args(addition_arguments_lists()));
        pp.setArguments(args);
    }
    pp.resolveAll();

    return true;
}

bool CookNinjaStep::init(QList<const BuildStep *> & earlierSteps)
{
    setIgnoreReturnValue(clean_);

    // get the build configuration
    BuildConfiguration *bc = qobject_cast<BuildConfiguration *>(buildConfiguration());
    if (!bc)
        bc = qobject_cast<BuildConfiguration *>(target()->activeBuildConfiguration());
    if (!bc)
        emit addTask(ProjectExplorer::Task::buildConfigurationMissingTask());

    if (!bc)
    {
        emitFaultyConfigurationMessage();
        return false;
    }

    // get the tool
    const toolset::CookTool * tool = bc->tool();
    if (!tool || !tool->is_valid())
    {
        ProjectExplorer::Task task(ProjectExplorer::Task::Error, QCoreApplication::translate("ProjectExplorer::Task", "No valid Cook tool specified. Configure the cook tool in the kit."), Utils::FileName(), -1, ProjectExplorer::Constants::TASK_CATEGORY_BUILDSYSTEM);
        emit addTask(task);
        return false;
    }

    // get the uri
    const QString & uri = bc->target_uri();
    if (!bc->find_recipe(uri))
    {
        ProjectExplorer::Task task(ProjectExplorer::Task::Error, QCoreApplication::translate("ProjectExplorer::Task", "No valid target specified."), Utils::FileName(), -1, ProjectExplorer::Constants::TASK_CATEGORY_BUILDSYSTEM);
        emit addTask(task);
        return false;
    }

    // make the build directory if necessary
    {
        Utils::FileName build_dir = bc->buildDirectory();
        if (!build_dir.exists())
        {
            if (!QDir(build_dir.toString()).mkpath("."))
            {
                ProjectExplorer::Task task(ProjectExplorer::Task::Error, QCoreApplication::translate("ProjectExplorer::Task", "Unable to create directory '%1'").arg(build_dir.toString()), Utils::FileName(), -1, ProjectExplorer::Constants::TASK_CATEGORY_BUILDSYSTEM);
                emit addTask(task);
                return false;
            }
        }
    }

    ProjectExplorer::ProcessParameters *pp = processParameters();
    pp->setMacroExpander(bc->macroExpander());
    pp->setWorkingDirectory(project()->projectDirectory().toString());
    Utils::Environment env = bc->environment();
    Utils::Environment::setupEnglishOutput(&env);
    pp->setEnvironment(env);
    pp->setCommand(tool->exec_file().absoluteFilePath());
    {
        QString args;
        Utils::QtcProcess::addArgs(&args, bc->ninja_build_args(addition_arguments_lists()));
        pp->setArguments(args);
    }

    pp->resolveAll();

    return ProjectExplorer::AbstractProcessStep::init(earlierSteps);
}

QStringList CookNinjaStep::addition_arguments_lists()
{
    if (additional_arguments_.isEmpty())
        return QStringList();
    else
        return additional_arguments_.split(' ');
}

void CookNinjaStep::run(QFutureInterface<bool> & interface)
{
    AbstractProcessStep::run(interface);
}

ProjectExplorer::BuildStepConfigWidget * CookNinjaStep::createConfigWidget()
{
    return new CookNinjaStepConfigWidget(this);
}

void CookNinjaStep::ctor_()
{
    setDisplayName("cook ninja step");
}

QVariantMap CookNinjaStep::toMap() const
{
    QVariantMap map = AbstractProcessStep::toMap();

    map.insert(ADDITIONAL_ARGUMENTS_KEY, additional_arguments_);
    map.insert(CLEAN_KEY, clean_);
    return map;
}

bool CookNinjaStep::fromMap(const QVariantMap &map)
{
    additional_arguments_ = map.value(ADDITIONAL_ARGUMENTS_KEY).toString();
    clean_ = map.value(CLEAN_KEY).toBool();

    return BuildStep::fromMap(map);
}

void CookNinjaStep::set_additional_arguments(const QString & list)
{
    if (list == additional_arguments_)
        return;

    additional_arguments_ = list;

    emit additional_arguments_changed(additional_arguments_);
}

QString CookNinjaStep::additional_arguments() const
{
    return additional_arguments_;
}

} }
