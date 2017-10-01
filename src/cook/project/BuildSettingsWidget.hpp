#ifndef HEADER_cook_project_BuildConfigurationWidget_hpp_ALREADY_INCLUDED
#define HEADER_cook_project_BuildConfigurationWidget_hpp_ALREADY_INCLUDED

#include <projectexplorer/namedwidget.h>
#include <utils/pathchooser.h>
#include <QCheckBox>
#include <QListWidget>

namespace cook { namespace project {

class BuildConfiguration;

class BuildSettingsWidget : public ProjectExplorer::NamedWidget
{
    Q_OBJECT

public:
    BuildSettingsWidget(BuildConfiguration *bc);

private:
    void targets_changed();
    void target_changed();
    void item_changed(QListWidgetItem *item);


    void buildDirectoryChanged();
    void environmentHasChanged();

    QCheckBox * shadow_build_box;
    Utils::PathChooser * build_dir_chooser;
    BuildConfiguration *bc_;
    QListWidget * build_targets_;
};

} }

#endif
