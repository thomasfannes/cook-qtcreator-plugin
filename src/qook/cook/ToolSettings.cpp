#include "qook/cook/ToolSettings.hpp"
#include "qook/cook/ToolSelection.hpp"
#include "qook/Constants.hpp"

#include <projectexplorer/projectexplorerconstants.h>
#include <projectexplorer/project.h>
#include <utils/persistentsettings.h>
#include <utils/fileutils.h>
#include <coreplugin/icore.h>

namespace qook { namespace cook {

namespace  {

Utils::FileName user_settings_file()
{
    QFileInfo settingsLocation(Core::ICore::settings()->fileName());
    return Utils::FileName::fromString(settingsLocation.absolutePath() + "/qtcreator/cooktool.xml");
}

}

ToolSettings::ToolSettings()
{
    setId(constants::QOOK_TOOLSETTINGS_ID);
    setDisplayName("Cook");
    setCategory(ProjectExplorer::Constants::PROJECTEXPLORER_SETTINGS_CATEGORY);
    setDisplayCategory(QCoreApplication::translate("ProjectExplorer", ProjectExplorer::Constants::PROJECTEXPLORER_SETTINGS_TR_CATEGORY));
    setCategoryIcon(Utils::Icon(ProjectExplorer::Constants::PROJECTEXPLORER_SETTINGS_CATEGORY_ICON));
}

QWidget * ToolSettings::widget()
{
    if (!widget_)
        widget_ = new ToolSelection;

    return widget_;
}

void ToolSettings::apply()
{
    // store the settings
//    Utils::PersistentSettingsWriter * w = new Utils::PersistentSettingsWriter()

}

void ToolSettings::finish()
{
    delete widget_;
    widget_= 0;
}

} }
