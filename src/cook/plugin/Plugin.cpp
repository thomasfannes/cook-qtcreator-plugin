#include "cook/plugin/Plugin.hpp"
#include "cook/chai/editor/Editor.hpp"
#include "cook/toolset/Settings.hpp"
#include "cook/toolset/Manager.hpp"
#include "cook/toolset/NinjaTool.hpp"
#include "cook/toolset/CookTool.hpp"
#include "cook/toolset/KitInformation.hpp"
#include "cook/project/Project.hpp"
#include "cook/project/BuildConfigurationFactory.hpp"
#include "cook/project/RunConfigurationFactory.hpp"
#include "cook/project/NinjaBuildStepFactory.hpp"
#include "cook/Constants.hpp"
#include <coreplugin/icore.h>

#include <coreplugin/icontext.h>
#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/actionmanager/command.h>
#include <coreplugin/actionmanager/actioncontainer.h>
#include <coreplugin/coreconstants.h>
#include <coreplugin/iwizardfactory.h>
#include <coreplugin/fileiconprovider.h>

#include <projectexplorer/projectmanager.h>
#include <projectexplorer/projectmanager.h>


#include <QAction>
#include <QMessageBox>
#include <QMainWindow>
#include <QMenu>

namespace cook { namespace plugin {


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

    ProjectExplorer::ProjectManager::registerProjectType<project::Project>(cook::constants::COOK_MIME_TYPE);
//    Core::IWizardFactory::registerFactoryCreator([]() { return QList<Core::IWizardFactory *>() << new ProjectWizardFactory; });


    cook::toolset::Manager::create_instance(this);

    // the cook tools
    {
        const Core::Id & type_id = toolset::CookTool::type_id();
        const QString name = "cook";

        cook::toolset::Manager::instance()->register_factory(type_id, new toolset::CookFactory);
        ProjectExplorer::KitManager::registerKitInformation(new cook::toolset::KitInformation(type_id, name));
        addAutoReleasedObject(new toolset::Settings(type_id, name));
    }

    // the ninja tools
    {
        const Core::Id & type_id = toolset::NinjaTool::type_id();
        const QString name = "ninja";

        cook::toolset::Manager::instance()->register_factory(type_id, new toolset::NinjaFactory);
        ProjectExplorer::KitManager::registerKitInformation(new cook::toolset::KitInformation(type_id, name));
        addAutoReleasedObject(new toolset::Settings(type_id, name));
    }


    addAutoReleasedObject(new chai::editor::EditorFactory);
    addAutoReleasedObject(new project::NinjaBuildStepFactory);
    addAutoReleasedObject(new project::BuildConfigurationFactory);
    addAutoReleasedObject(new project::RunConfigurationFactory);

    return true;
}

void Plugin::extensionsInitialized()
{
    cook::toolset::Manager::instance()->restore();
}


} }

