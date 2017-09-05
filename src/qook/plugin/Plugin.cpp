#include "qook/plugin/Plugin.hpp"
#include "qook/chai/editor/Editor.hpp"
#include "qook/cook/ToolSettings.hpp"
#include "qook/QookProject.hpp"
#include "qook/ProjectWizard.hpp"
#include "qook/Constants.hpp"

#include <coreplugin/icore.h>
#include <coreplugin/icontext.h>
#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/actionmanager/command.h>
#include <coreplugin/actionmanager/actioncontainer.h>
#include <coreplugin/coreconstants.h>

#include <projectexplorer/projectmanager.h>
#include <projectexplorer/projectmanager.h>
#include <coreplugin/iwizardfactory.h>

#include <QAction>
#include <QMessageBox>
#include <QMainWindow>
#include <QMenu>

namespace qook { namespace plugin {


Plugin::Plugin()
{

}

Plugin::~Plugin()
{

}

bool Plugin::initialize(const QStringList &arguments, QString *errorString)
{
    // Register objects in the plugin manager's object pool
    // Load settings
    // Add actions to menus
    // Connect to other plugins' signals
    // In the initialize function, a plugin can be sure that the plugins it
    // depends on have initialized their members.
    Q_UNUSED(arguments)
    Q_UNUSED(errorString)

    ProjectExplorer::ProjectManager::registerProjectType<QookProject>(qook::constants::CHAI_MIME_TYPE);
//    Core::IWizardFactory::registerFactoryCreator([]() { return QList<Core::IWizardFactory *>() << new ProjectWizardFactory; });

    addAutoReleasedObject(new chai::editor::EditorFactory);
    addAutoReleasedObject(new cook::ToolSettings);


    /*auto action = new QAction(tr("QookPlugin Action"), this);
    Core::Command *cmd = Core::ActionManager::registerAction(action, constants::ACTION_ID,
                                                             Core::Context(Core::Constants::C_GLOBAL));
    cmd->setDefaultKeySequence(QKeySequence(tr("Ctrl+Alt+Meta+A")));
    connect(action, &QAction::triggered, this, &Plugin::triggerAction);

    Core::ActionContainer *menu = Core::ActionManager::createMenu(constants::MENU_ID);
    menu->menu()->setTitle(tr("QookPlugin"));
    menu->addAction(cmd);
    Core::ActionManager::actionContainer(Core::Constants::M_TOOLS)->addMenu(menu);*/

    return true;
}

void Plugin::extensionsInitialized()
{

}

void Plugin::triggerAction()
{
    QMessageBox::information(Core::ICore::mainWindow(),
                             tr("Action Triggered"),
                             tr("This is an action from QookPlugin."));
}

} }

