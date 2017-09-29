#include "qook/project/RunConfiguration.hpp"
#include "qook/project/RunSettingsWidget.hpp"
#include "qook/project/Project.hpp"
#include <projectexplorer/localenvironmentaspect.h>
#include <projectexplorer/target.h>
#include <projectexplorer/runconfigurationaspects.h>
#include <projectexplorer/runnables.h>

namespace qook { namespace project {

namespace  {

const char CMAKE_RC_PREFIX[] = "CookProjectManager.CookRunConfiguration.";
const char TITLE_KEY[] = "CookProjectManager.CookRunConfiguation.Title";

}

RunConfiguration::RunConfiguration(ProjectExplorer::Target * parent, Core::Id id, const CookBuildTarget & target)
    : ProjectExplorer::RunConfiguration(parent, id),
      target_(target)
{
    addExtraAspect(new ProjectExplorer::LocalEnvironmentAspect(this, ProjectExplorer::LocalEnvironmentAspect::BaseEnvironmentModifier()));
    addExtraAspect(new ProjectExplorer::ArgumentsAspect(this, QStringLiteral("CookProjectManager.CookRunConfiguration.Arguments")));
    addExtraAspect(new ProjectExplorer::TerminalAspect(this, QStringLiteral("CookProjectManager.CookRunConfiguration.UseTerminal")));

    auto wd = new ProjectExplorer::WorkingDirectoryAspect(this, QStringLiteral("CookProjectManager.CookRunConfiguration.UserWorkingDirectory"));
    wd->setDefaultWorkingDirectory(parent->project()->projectDirectory());
    addExtraAspect(wd);

    ctor();
}

RunConfiguration::RunConfiguration(ProjectExplorer::Target *parent, RunConfiguration * source)
    : ProjectExplorer::RunConfiguration(parent, source),
      target_(source->target_)
{
    ctor();
}

void RunConfiguration::ctor()
{
    setDefaultDisplayName(default_display_name_());
}

QString RunConfiguration::default_display_name_() const
{
    if(target_.display_name.isEmpty())
        return QString("undefined");

    return target_.display_name;
}


ProjectExplorer::Runnable RunConfiguration::runnable() const
{
    ProjectExplorer::StandardRunnable r;

    r.executable = target_.executable.toString();
    r.commandLineArguments = extraAspect<ProjectExplorer::ArgumentsAspect>()->arguments();
    r.workingDirectory = extraAspect<ProjectExplorer::WorkingDirectoryAspect>()->workingDirectory().toString();
    r.environment = extraAspect<ProjectExplorer::LocalEnvironmentAspect>()->environment();
    r.runMode = extraAspect<ProjectExplorer::TerminalAspect>()->runMode();

    return r;
}
//ProjectExplorer::Runnable RunConfiguration::runnable() const
//{
//    ProjectExplorer::StandardRunnable r;
//    r.executable = executable_;
//    r.commandLineArguments = extraAspect<ProjectExplorer::ArgumentsAspect>()->arguments();
//    r.workingDirectory = extraAspect<ProjectExplorer::WorkingDirectoryAspect>()->workingDirectory().toString();
//    r.environment = extraAspect<ProjectExplorer::LocalEnvironmentAspect>()->environment();
//    r.runMode = extraAspect<ProjectExplorer::TerminalAspect>()->runMode();

//    return r;
//}

QWidget * RunConfiguration::createConfigurationWidget()
{
    return new RunSettingsWidget(this);
}

//void RunConfiguration::setExecutable(const QString &executable)
//{
//    executable_ = executable;
//}

//void RunConfiguration::setBaseWorkingDirectory(const Utils::FileName & wd)
//{
//    extraAspect<ProjectExplorer::WorkingDirectoryAspect>()->setDefaultWorkingDirectory(wd);
//}

//QString RunConfiguration::title() const
//{
//    return title_;
//}

//QVariantMap RunConfiguration::toMap() const
//{
//    QVariantMap map(ProjectExplorer::RunConfiguration::toMap());
//    map.insert(QLatin1String(TITLE_KEY), title_);
//    return map;
//}

//QString RunConfiguration::disabledReason() const
//{
//    auto cp = qobject_cast<Project *>(target()->project());
//    QTC_ASSERT(cp, return QString());

////    if (!cp->has_built_target(build_system_target_))
////        return tr("The project no longer builds the target associated with this run configuration.");
//    return RunConfiguration::disabledReason();
//}

//QString RunConfiguration::buildSystemTarget() const
//{
//    return build_system_target_.uri;
//}

//bool RunConfiguration::fromMap(const QVariantMap &map)
//{
//    title_ = map.value(QLatin1String(TITLE_KEY)).toString();
//    return RunConfiguration::fromMap(map);
//}

//QString RunConfiguration::defaultDisplayName() const
//{
//    if (title_.isEmpty())
//        return tr("Run Cook kit");
//    return title_;
//}

//void RunConfiguration::updateEnabledState()
//{
//    auto cp = qobject_cast<Project *>(target()->project());
////    if (!cp->has_built_target(build_system_target_))
////        setEnabled(false);
////    else
////
//    RunConfiguration::updateEnabledState();
//}

//QString RunConfiguration::baseWorkingDirectory() const
//{
//    const QString exe = executable_;
//    if (!exe.isEmpty())
//        return QFileInfo(executable_).absolutePath();
//    return QString();
//}




} }
