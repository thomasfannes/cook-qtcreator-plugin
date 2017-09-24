#include "qook/project/CookBuildStepConfigWidget.hpp"
#include "qook/project/CookBuildStep.hpp"
#include "qook/project/Project.hpp"

#include <coreplugin/find/itemviewfind.h>
#include <QFormLayout>
#include <QListWidgetItem>
#include <QDebug>

namespace qook { namespace project {

CookBuildStepConfigWidget::CookBuildStepConfigWidget(CookBuildStep * build_step)
    : build_step_(build_step),
      build_targets_(new QListWidget)
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

    connect(build_step, &CookBuildStep::build_targets_changed, this, &CookBuildStepConfigWidget::build_targets_changed);
    connect(build_step, &CookBuildStep::build_target_changed, this, &CookBuildStepConfigWidget::build_target_changed);
    connect(build_targets_, &QListWidget::itemChanged, this, &CookBuildStepConfigWidget::item_changed);

    build_targets_changed();

}

QString CookBuildStepConfigWidget::displayName() const
{
    return tr("Build", "CookProjectManager::CookStepConfigWidget display name.");
}

void CookBuildStepConfigWidget::build_targets_changed()
{
    const bool was_locked = build_targets_->blockSignals(true);
    build_targets_->clear();

    QFont italics;
    italics.setItalic(true);

    // add all the targets
    for(const auto & tgt : build_step_->all_targets())
    {
        auto item = new QListWidgetItem(tgt.display_name, build_targets_);
        if(tgt.is_special)
            item->setFont(italics);

        // set the value
        QVariant var;
        var.setValue(tgt);
        item->setData(Qt::UserRole, var);
        item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
    }

    build_target_changed();
    build_targets_->blockSignals(was_locked);
}

void CookBuildStepConfigWidget::item_changed(QListWidgetItem * item)
{
    const CookBuildTarget tgt = (item->checkState() == Qt::Checked ? item->data(Qt::UserRole).value<CookBuildTarget>() : CookBuildTarget::all());
    build_step_->set_build_target_(tgt);
    update_details();
}

void CookBuildStepConfigWidget::build_target_changed()
{
    const bool wasBlocked = build_targets_->blockSignals(true);

    for (int i = 0; i < build_targets_->count(); ++i)
    {
        QListWidgetItem *item = build_targets_->item(i);
        const CookBuildTarget & tgt = item->data(Qt::UserRole).value<CookBuildTarget>();

        item->setCheckState(build_step_->builds_target(tgt) ? Qt::Checked : Qt::Unchecked);
    }
    build_targets_->blockSignals(wasBlocked);
    update_details();
}

void CookBuildStepConfigWidget::update_details()
{
    ProjectExplorer::ProcessParameters param;
    build_step_->configure_process_parameters_(param, nullptr, false);
    summary_text_ = param.summary(displayName());

    emit updateSummary();
}


} }
