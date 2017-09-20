#include "qook/project/BuildSettingsWidget.hpp"
#include "qook/project/BuildConfiguration.hpp"
#include "projectexplorer/target.h"
#include "projectexplorer/project.h"
#include "projectexplorer/buildconfiguration.h"
#include <QFormLayout>

namespace qook { namespace project {

BuildSettingsWidget::BuildSettingsWidget(BuildConfiguration *bc)
    : m_buildConfiguration(0)
{
    auto fl = new QFormLayout(this);
    fl->setContentsMargins(0, -1, 0, -1);
    fl->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);

    // shadow build
    shadow_build_box = new QCheckBox(this);
    shadow_build_box->setEnabled(true);
    fl->addRow(tr("Shadow build:"), shadow_build_box);

    // build directory
    build_dir_chooser = new Utils::PathChooser(this);
    build_dir_chooser->setHistoryCompleter("Cook.BuildDir.History");
    build_dir_chooser->setEnabled(true);
    fl->addRow(tr("Build directory:"), build_dir_chooser);
    connect(build_dir_chooser, &Utils::PathChooser::rawPathChanged, this, &BuildSettingsWidget::buildDirectoryChanged);

    m_buildConfiguration = bc;
    build_dir_chooser->setBaseFileName(bc->target()->project()->projectDirectory());
    build_dir_chooser->setEnvironment(bc->environment());
    build_dir_chooser->setPath(m_buildConfiguration->rawBuildDirectory().toString());
    setDisplayName(tr("Cook Build Manager"));

    connect(bc, &BuildConfiguration::environmentChanged, this, &BuildSettingsWidget::environmentHasChanged);
}

void BuildSettingsWidget::buildDirectoryChanged()
{
    m_buildConfiguration->setBuildDirectory(Utils::FileName::fromString(build_dir_chooser->rawPath()));
}

void BuildSettingsWidget::environmentHasChanged()
{
    build_dir_chooser->setEnvironment(m_buildConfiguration->environment());
}

} }
