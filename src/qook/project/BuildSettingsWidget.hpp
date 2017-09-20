#ifndef HEADER_qook_project_BuildConfigurationWidget_hpp_ALREADY_INCLUDED
#define HEADER_qook_project_BuildConfigurationWidget_hpp_ALREADY_INCLUDED

#include <projectexplorer/namedwidget.h>
#include <utils/pathchooser.h>
#include <QCheckBox>

namespace qook { namespace project {

class BuildConfiguration;

class BuildSettingsWidget : public ProjectExplorer::NamedWidget
{
    Q_OBJECT

public:
    BuildSettingsWidget(BuildConfiguration *bc);

private:
    void buildDirectoryChanged();
    void environmentHasChanged();

    QCheckBox * shadow_build_box;
    Utils::PathChooser * build_dir_chooser;
    BuildConfiguration *m_buildConfiguration;
};

} }

#endif
