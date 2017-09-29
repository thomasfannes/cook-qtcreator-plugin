#include "qook/plugin/Plugin.hpp"
#include "qook/chai/editor/Editor.hpp"
#include "qook/toolset/Settings.hpp"
#include "qook/toolset/Manager.hpp"
#include "qook/toolset/NinjaTool.hpp"
#include "qook/toolset/CookTool.hpp"
#include "qook/toolset/KitInformation.hpp"
#include "qook/project/Project.hpp"
#include "qook/project/BuildConfigurationFactory.hpp"
#include "qook/project/RunConfigurationFactory.hpp"
#include "qook/project/CookBuildStepFactory.hpp"
#include "qook/project/NinjaBuildStepFactory.hpp"
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
#include <coreplugin/fileiconprovider.h>

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

    Core::FileIconProvider::registerIconOverlayForSuffix(constants::FILE_OVERLAY_CHAI, "chai");

    ProjectExplorer::ProjectManager::registerProjectType<project::Project>(qook::constants::QOOK_MIME_TYPE);
//    Core::IWizardFactory::registerFactoryCreator([]() { return QList<Core::IWizardFactory *>() << new ProjectWizardFactory; });


    qook::toolset::Manager::create_instance(this);

    // the cook tools
    {
        const Core::Id & type_id = toolset::CookTool::type_id();
        const QString name = "cook";

        qook::toolset::Manager::instance()->register_factory(type_id, new toolset::CookFactory);
        ProjectExplorer::KitManager::registerKitInformation(new qook::toolset::KitInformation(type_id, name));
        addAutoReleasedObject(new toolset::Settings(type_id, name));
    }

    // the ninja tools
    {
        const Core::Id & type_id = toolset::NinjaTool::type_id();
        const QString name = "ninja";

        qook::toolset::Manager::instance()->register_factory(type_id, new toolset::NinjaFactory);
        ProjectExplorer::KitManager::registerKitInformation(new qook::toolset::KitInformation(type_id, name));
        addAutoReleasedObject(new toolset::Settings(type_id, name));
    }


    addAutoReleasedObject(new chai::editor::EditorFactory);
    addAutoReleasedObject(new project::CookBuildStepFactory);
    addAutoReleasedObject(new project::NinjaBuildStepFactory);
    addAutoReleasedObject(new project::BuildConfigurationFactory);
    addAutoReleasedObject(new project::RunConfigurationFactory);


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
    qook::toolset::Manager::instance()->restore();
}

void Plugin::triggerAction()
{
    QMessageBox::information(Core::ICore::mainWindow(),
                             tr("Action Triggered"),
                             tr("This is an action from QookPlugin."));
}

} }

