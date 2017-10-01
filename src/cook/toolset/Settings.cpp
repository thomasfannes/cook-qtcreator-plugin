#include "cook/toolset/Settings.hpp"
#include "cook/toolset/SettingsWidget.hpp"
#include "cook/toolset/ConfigItemModel.hpp"
#include "cook/Constants.hpp"

#include <projectexplorer/projectexplorerconstants.h>
#include <projectexplorer/project.h>
#include <utils/persistentsettings.h>
#include <utils/fileutils.h>
#include <coreplugin/icore.h>

namespace cook { namespace toolset {

Settings::Settings(const Core::Id & type_id, const QString & name)
    : type_id_(type_id),
      name_(name)
{
    Core::Id id(constants::COOK_TOOLSET_SETTINGS_ID);

    setId(id.withSuffix(name_));
    setDisplayName(name_);
    setCategory(ProjectExplorer::Constants::PROJECTEXPLORER_SETTINGS_CATEGORY);
    setDisplayCategory(QCoreApplication::translate("ProjectExplorer", ProjectExplorer::Constants::PROJECTEXPLORER_SETTINGS_TR_CATEGORY));
    setCategoryIcon(Utils::Icon(ProjectExplorer::Constants::PROJECTEXPLORER_SETTINGS_CATEGORY_ICON));
}

QWidget * Settings::widget()
{
    if (!widget_)
        widget_ = new SettingsWidget(type_id_, name_);

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

