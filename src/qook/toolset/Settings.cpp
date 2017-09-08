#include "qook/toolset/Settings.hpp"
#include "qook/toolset/SettingsWidget.hpp"
#include "qook/toolset/ConfigItemModel.hpp"
#include "qook/Constants.hpp"

#include <projectexplorer/projectexplorerconstants.h>
#include <projectexplorer/project.h>
#include <utils/persistentsettings.h>
#include <utils/fileutils.h>
#include <coreplugin/icore.h>

namespace qook { namespace toolset {

Settings::Settings()
{
    setId(constants::QOOK_TOOLSETTINGS_ID);
    setDisplayName("Cook");
    setCategory(ProjectExplorer::Constants::PROJECTEXPLORER_SETTINGS_CATEGORY);
    setDisplayCategory(QCoreApplication::translate("ProjectExplorer", ProjectExplorer::Constants::PROJECTEXPLORER_SETTINGS_TR_CATEGORY));
    setCategoryIcon(Utils::Icon(ProjectExplorer::Constants::PROJECTEXPLORER_SETTINGS_CATEGORY_ICON));
}

QWidget * Settings::widget()
{
    if (!widget_)
        widget_ = new SettingsWidget;

    return widget_;
}

void Settings::apply()
{
    // store the settings
    widget_->model()->apply();
}

void Settings::finish()
{
    delete widget_;
    widget_= 0;
}

} }

