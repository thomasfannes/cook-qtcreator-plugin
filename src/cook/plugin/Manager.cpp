#include "cook/plugin/Manager.hpp"
#include "cook/project/Project.hpp"
#include "cook/Constants.hpp"

#include <projectexplorer/projectexplorerconstants.h>
#include <projectexplorer/session.h>
#include <projectexplorer/buildmanager.h>
#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/actionmanager/actioncontainer.h>

#include <utils/qtcassert.h>
#include <QDebug>

namespace cook { namespace plugin {

namespace {

const char RESCAN_PROJECT[] = "CookPlugin.RescanProject";
const char RUN_COOK_CONTEXT[] = "CookPplugin.RunCook.Context";

}

Manager::Manager()
    : rescan_project_(new QAction(QIcon(), tr("Rescan recipes"), this))
{
    const Core::Context globalContext(Core::Constants::C_GLOBAL);
    const Core::Context projectContext(constants::COOK_PROJECT_CONTEXT);


    Core::ActionContainer *mbuild       = Core::ActionManager::actionContainer(ProjectExplorer::Constants::M_BUILDPROJECT);
    Core::ActionContainer *mproject     = Core::ActionManager::actionContainer(ProjectExplorer::Constants::M_PROJECTCONTEXT);

    // add the menu action
    {
        Core::Command * command = Core::ActionManager::registerAction(rescan_project_, RESCAN_PROJECT, globalContext);
        command->setAttribute(Core::Command::CA_Hide);
        mbuild->addAction(command, ProjectExplorer::Constants::G_BUILD_DEPLOY);
    }

    // add the context menu action
    {
        Core::Command * command = Core::ActionManager::registerAction(rescan_project_, RUN_COOK_CONTEXT, projectContext);
        command->setAttribute(Core::Command::CA_Hide);
        mproject->addAction(command, ProjectExplorer::Constants::G_PROJECT_BUILD);
    }

    connect(rescan_project_, &QAction::triggered, this, &Manager::rescan_active_project_);
    connect(ProjectExplorer::SessionManager::instance(), &ProjectExplorer::SessionManager::startupProjectChanged, this, &Manager::update_actions_);
    connect(ProjectExplorer::BuildManager::instance(), &ProjectExplorer::BuildManager::buildStateChanged, this, &Manager::update_actions_);

}

void Manager::update_actions_()
{
    project::Project * project = qobject_cast<project::Project *>(ProjectExplorer::SessionManager::startupProject());
    const bool visible = project && !ProjectExplorer::BuildManager::isBuilding(project);
    rescan_project_->setVisible(visible);
}

void Manager::rescan_active_project_()
{
    project::Project * project = qobject_cast<project::Project *>(ProjectExplorer::SessionManager::startupProject());
    QTC_ASSERT(project, return);

    project->refresh_all();
}

} }
