#include "cook/project/BuildSettingsWidget.hpp"
#include "cook/project/BuildConfiguration.hpp"
#include "projectexplorer/target.h"
#include "projectexplorer/project.h"
#include "projectexplorer/buildconfiguration.h"
#include <QFormLayout>
#include <coreplugin/find/itemviewfind.h>

namespace cook { namespace project {

BuildSettingsWidget::BuildSettingsWidget(BuildConfiguration *bc)
    : bc_(bc),
      build_targets_(new QListWidget(this))
{
    auto fl = new QFormLayout();
    fl->setMargin(0);
    fl->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);
    setLayout(fl);

    //    build_targets_->setFrameStyle(QFrame::NoFrame);
    build_targets_->setMinimumHeight(200);
    build_targets_->setSelectionMode(QAbstractItemView::SingleSelection);

    auto frame = new QFrame(this);
    frame->setFrameStyle(QFrame::StyledPanel);
    auto frameLayout = new QVBoxLayout(frame);
    frameLayout->setMargin(0);
    frameLayout->addWidget(Core::ItemViewFind::createSearchableWrapper(build_targets_/*, Core::ItemViewFind::LightColored*/));

    fl->addRow(tr("Targets:"), frame);

    connect(bc, &BuildConfiguration::build_targets_changed, this, &BuildSettingsWidget::targets_changed);
    connect(bc, &BuildConfiguration::build_target_changed, this, &BuildSettingsWidget::target_changed);
    connect(build_targets_, &QListWidget::itemChanged, this, &BuildSettingsWidget::item_changed);

//    connect(build_step, &CookBuildStep::build_targets_changed, this, &CookBuildStepConfigWidget::build_targets_changed);
//    connect(build_step, &CookBuildStep::build_target_changed, this, &CookBuildStepConfigWidget::build_target_changed);


//    auto fl = new QFormLayout(this);
//    fl->setContentsMargins(0, -1, 0, -1);
//    fl->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);

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

    build_dir_chooser->setBaseFileName(bc->target()->project()->projectDirectory());
    build_dir_chooser->setEnvironment(bc->environment());
    build_dir_chooser->setPath(bc_->rawBuildDirectory().toString());
    setDisplayName(tr("Cook Build Manager"));

    connect(bc, &BuildConfiguration::environmentChanged, this, &BuildSettingsWidget::environmentHasChanged);
}

void BuildSettingsWidget::targets_changed()
{
    const bool was_locked = build_targets_->blockSignals(true);
    build_targets_->clear();

    QFont italics;
    italics.setItalic(true);

    // add all the targets
    for(const auto & tgt : bc_->all_build_targets())
    {
        auto item = new QListWidgetItem(tgt.display_name, build_targets_);
        if(tgt.type != Target_URI)
            item->setFont(italics);

        // set the value
        QVariant var;
        var.setValue(tgt);
        item->setData(Qt::UserRole, var);
        item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
    }

    target_changed();
    build_targets_->blockSignals(was_locked);
}

void BuildSettingsWidget::target_changed()
{
    const bool wasBlocked = build_targets_->blockSignals(true);

    for (int i = 0; i < build_targets_->count(); ++i)
    {
        QListWidgetItem *item = build_targets_->item(i);
        const CookBuildTarget & tgt = item->data(Qt::UserRole).value<CookBuildTarget>();
        item->setCheckState(bc_->build_target() == tgt ? Qt::Checked : Qt::Unchecked);
    }
    build_targets_->blockSignals(wasBlocked);
}

void BuildSettingsWidget::item_changed(QListWidgetItem * item)
{
    const CookBuildTarget tgt = (item->checkState() == Qt::Checked ? item->data(Qt::UserRole).value<CookBuildTarget>() : CookBuildTarget::default_target());
    bc_->set_build_target(tgt);
}

void BuildSettingsWidget::buildDirectoryChanged()
{
    bc_->setBuildDirectory(Utils::FileName::fromString(build_dir_chooser->rawPath()));
}

void BuildSettingsWidget::environmentHasChanged()
{
    build_dir_chooser->setEnvironment(bc_->environment());
}


} }
