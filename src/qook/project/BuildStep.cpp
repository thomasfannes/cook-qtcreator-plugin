#include "qook/project/BuildStep.hpp"
#include "qook/project/BuildConfiguration.hpp"
#include "qook/project/Project.hpp"
#include "qook/Constants.hpp"
#include <projectexplorer/target.h>
#include <projectexplorer/projectexplorerconstants.h>
#include <utils/qtcassert.h>

namespace qook { namespace project {

namespace {
const char CLEAN_KEY[] = "CookProjectManager.MakeStep.Clean"; // Obsolete since QtC 3.7
const char BUILD_TARGETS_KEY[] = "CookProjectManager.MakeStep.BuildTargets";
const char TOOL_ARGUMENTS_KEY[] = "ookProjectManager.MakeStep.AdditionalArguments";
const char ADD_RUNCONFIGURATION_ARGUMENT_KEY[] = "CookProjectManager.MakeStep.AddRunConfigurationArgument";
const char ADD_RUNCONFIGURATION_TEXT[] = "Current executable";
}

static bool isCurrentExecutableTarget(const QString &target)
{
    return target == ADD_RUNCONFIGURATION_TEXT;
}

class CookStepConfigWidget : public ProjectExplorer::BuildStepConfigWidget
{
    virtual QString summaryText() const { return "summaryText"; }
    virtual QString displayName() const { return "displayName"; }
};

CookBuildStep::CookBuildStep(ProjectExplorer::BuildStepList *parent)
    :AbstractProcessStep(parent, constants::COOK_BUILD_STEP_ID)
{
}

ProjectExplorer::BuildStepConfigWidget *CookBuildStep::createConfigWidget()
{
    return new CookStepConfigWidget();
}

/*BuildStep::BuildStep(ProjectExplorer::BuildStepList *bsl) :
    AbstractProcessStep(bsl, Core::Id(constants::COOK_BUILD_STEP_ID))
{
    ctor(bsl);
}

BuildStep::BuildStep(ProjectExplorer::BuildStepList *bsl, Core::Id id)
    : AbstractProcessStep(bsl, id)
{
    ctor(bsl);
}

BuildStep::BuildStep(ProjectExplorer::BuildStepList *bsl, BuildStep * bs)
    : AbstractProcessStep(bsl, bs),
      m_buildTarget(bs->m_buildTarget),
      m_toolArguments(bs->m_toolArguments)
{
    ctor(bsl);
}

void BuildStep::ctor(ProjectExplorer::BuildStepList *bsl)
{
    m_percentProgress = QRegExp("^\\[\\s*(\\d*)%\\]");
    m_ninjaProgress = QRegExp("^\\[\\s*(\\d*)/\\s*(\\d*)");
    m_ninjaProgressString = "[%f/%t "; // ninja: [33/100
    //: Default display name for the cmake make step.
    setDefaultDisplayName(tr("Cook Build"));

    auto bc = qobject_cast<BuildConfiguration *>(bsl->parent());
    if (!bc)
    {
        auto t = qobject_cast<ProjectExplorer::Target *>(bsl->parent()->parent());
        QTC_ASSERT(t, return);
        bc = qobject_cast<BuildConfiguration *>(t->activeBuildConfiguration());
    }

    // Set a good default build target:
    if (m_buildTarget.isEmpty())
    {
        if (bsl->id() == ProjectExplorer::Constants::BUILDSTEPS_CLEAN)
            setBuildTarget(cleanTarget());
        else if (bsl->id() == ProjectExplorer::Constants::BUILDSTEPS_DEPLOY)
            setBuildTarget(installTarget());
        else
            setBuildTarget(allTarget());
    }

    connect(target(), &ProjectExplorer::Target::kitChanged, this, &BuildStep::toolset_changed);
    connect(project(), &ProjectExplorer::Project::parsingFinished, this, &BuildStep::handle_build_target_changes);
}

BuildConfiguration * BuildStep::buildConfiguration() const
{
    return static_cast<BuildConfiguration *>(buildConfiguration());
}

BuildConfiguration * BuildStep::targetsActiveBuildConfiguration() const
{
    return static_cast<BuildConfiguration *>(target()->activeBuildConfiguration());
}

RunConfiguration * BuildStep::targetsActiveRunConfiguration() const
{
    return qobject_cast<RunConfiguration *>(target()->activeRunConfiguration());
}

void BuildStep::handle_build_target_changes()
{
    if (isCurrentExecutableTarget(m_buildTarget))
        return; // Do not change just because a different set of build targets is there...

    // target does not exist anymore
    if (!targetsActiveBuildConfiguration()->build_targets().contains(m_buildTarget))
        setBuildTarget(allTarget());

    emit buildTargetsChanged();
}

QVariantMap BuildStep::toMap() const
{
    QVariantMap map(AbstractProcessStep::toMap());

    // Use QStringList for compatibility with old files
    map.insert(BUILD_TARGETS_KEY, QStringList(m_buildTarget));
    map.insert(TOOL_ARGUMENTS_KEY, m_toolArguments);
    return map;
}

bool BuildStep::fromMap(const QVariantMap &map)
{
    const QStringList targetList = map.value(BUILD_TARGETS_KEY).toStringList();
    if (!targetList.isEmpty())
        m_buildTarget = targetList.last();

    m_toolArguments = map.value(TOOL_ARGUMENTS_KEY).toString();

    if (map.value(ADD_RUNCONFIGURATION_ARGUMENT_KEY, false).toBool())
        m_buildTarget = ADD_RUNCONFIGURATION_TEXT;

    return BuildStep::fromMap(map);
}


bool BuildStep::init(QList<const BuildStep *> &earlierSteps)
{
    bool canInit = true;
    BuildConfiguration *bc = BuildConfiguration();

    if (!bc)
        bc = targetsActiveBuildConfiguration();

    if (!bc)
    {
        emit addTask(ProjectExplorer::Task::buildConfigurationMissingTask());
        canInit = false;
    }

    if (bc && !bc->isEnabled())
    {
        emit addTask(Task(ProjectExplorer::Task::Error, QCoreApplication::translate("CookProjectManager::BuildStep", "The build configuration is currently disabled."), Utils::FileName(), -1, ProjectExplorer::Constants::TASK_CATEGORY_BUILDSYSTEM));
        canInit = false;
    }

    CMakeTool *tool = CMakeKitInformation::cmakeTool(target()->kit());
    if (!tool || !tool->isValid()) {
        emit addTask(Task(Task::Error,
                          tr("A CMake tool must be set up for building. "
                             "Configure a CMake tool in the kit options."),
                          Utils::FileName(), -1,
                          ProjectExplorer::Constants::TASK_CATEGORY_BUILDSYSTEM));
        canInit = false;
    }

    CMakeRunConfiguration *rc = targetsActiveRunConfiguration();
    if (isCurrentExecutableTarget(m_buildTarget) && (!rc || rc->buildSystemTarget().isEmpty())) {
        emit addTask(Task(Task::Error,
                          QCoreApplication::translate("ProjectExplorer::Task",
                                    "You asked to build the current Run Configuration's build target only, "
                                    "but it is not associated with a build target. "
                                    "Update the Make Step in your build settings."),
                        Utils::FileName(), -1,
                        ProjectExplorer::Constants::TASK_CATEGORY_BUILDSYSTEM));
        canInit = false;
    }

    if (!canInit) {
        emitFaultyConfigurationMessage();
        return false;
    }

    // Warn if doing out-of-source builds with a CMakeCache.txt is the source directory
    const Utils::FileName projectDirectory = bc->target()->project()->projectDirectory();
    if (bc->buildDirectory() != projectDirectory) {
        Utils::FileName cmc = projectDirectory;
        cmc.appendPath("CMakeCache.txt");
        if (cmc.exists()) {
            emit addTask(Task(Task::Warning,
                              tr("There is a CMakeCache.txt file in \"%1\", which suggest an "
                                 "in-source build was done before. You are now building in \"%2\", "
                                 "and the CMakeCache.txt file might confuse CMake.")
                              .arg(projectDirectory.toUserOutput(), bc->buildDirectory().toUserOutput()),
                              Utils::FileName(), -1,
                              ProjectExplorer::Constants::TASK_CATEGORY_BUILDSYSTEM));
        }
    }

    QString arguments = allArguments(rc);

    setIgnoreReturnValue(m_buildTarget == BuildStep::cleanTarget());

    ProjectExplorer::ProcessParameters *pp = processParameters();
    pp->setMacroExpander(bc->macroExpander());
    Utils::Environment env = bc->environment();
    Utils::Environment::setupEnglishOutput(&env);
    if (!env.value("NINJA_STATUS").startsWith(m_ninjaProgressString))
        env.set("NINJA_STATUS", m_ninjaProgressString + "%o/sec] ");
    pp->setEnvironment(env);
    pp->setWorkingDirectory(bc->buildDirectory().toString());
    pp->setCommand(cmakeCommand());
    pp->setArguments(arguments);
    pp->resolveAll();

    setOutputParser(new CMakeParser);
    appendOutputParser(new GnuMakeParser);
    IOutputParser *parser = target()->kit()->createOutputParser();
    if (parser)
        appendOutputParser(parser);
    outputParser()->setWorkingDirectory(pp->effectiveWorkingDirectory());

    return AbstractProcessStep::init(earlierSteps);
}

void CMakeBuildStep::run(QFutureInterface<bool> &fi)
{
    // Make sure CMake state was written to disk before trying to build:
    CMakeBuildConfiguration *bc = cmakeBuildConfiguration();
    if (!bc)
        bc = targetsActiveBuildConfiguration();
    QTC_ASSERT(bc, return);

    bool mustDelay = false;
    if (bc->persistCMakeState()) {
        emit addOutput(tr("Persisting CMake state..."), BuildStep::OutputFormat::NormalMessage);
        mustDelay = true;
    } else if (bc->updateCMakeStateBeforeBuild()) {
        emit addOutput(tr("Running CMake in preparation to build..."), BuildStep::OutputFormat::NormalMessage);
        mustDelay = true;
    } else {
        mustDelay = false;
    }

    if (mustDelay) {
        m_runTrigger = connect(project(), &Project::parsingFinished,
                               this, [this, &fi](bool success) { handleProjectWasParsed(fi, success); });
    } else {
        runImpl(fi);
    }
}

void CMakeBuildStep::runImpl(QFutureInterface<bool> &fi)
{
    // Do the actual build:
    AbstractProcessStep::run(fi);
}

void CMakeBuildStep::handleProjectWasParsed(QFutureInterface<bool> &fi, bool success)
{
    disconnect(m_runTrigger);
    if (success) {
        runImpl(fi);
    } else {
        AbstractProcessStep::stdError(tr("Project did not parse successfully, can not build."));
        reportRunResult(fi, false);
    }
}

void BuildStep::stdOutput(const QString &line)
{
    if (m_percentProgress.indexIn(line) != -1)
    {
        AbstractProcessStep::stdOutput(line);
        bool ok = false;
        int percent = m_percentProgress.cap(1).toInt(&ok);
        if (ok)
            futureInterface()->setProgressValue(percent);
        return;
    } else if (m_ninjaProgress.indexIn(line) != -1)
    {
        AbstractProcessStep::stdOutput(line);
        m_useNinja = true;
        bool ok = false;
        int done = m_ninjaProgress.cap(1).toInt(&ok);
        if (ok)
        {
            int all = m_ninjaProgress.cap(2).toInt(&ok);
            if (ok && all != 0)
            {
                const int percent = static_cast<int>(100.0 * done/all);
                futureInterface()->setProgressValue(percent);
            }
        }
        return;
    }
    if (m_useNinja)
        AbstractProcessStep::stdError(line);
}*/

} }
