#include "qook/toolset/SettingsWidget.hpp"
#include "qook/toolset/Manager.hpp"
#include "qook/toolset/ConfigItemModel.hpp"
#include "qook/toolset/ConfigItem.hpp"
#include "qook/toolset/CookTool.hpp"

#include "ui_SettingsWidget.h"
#include <QFileDialog>
#include <QDebug>
#include <QItemSelectionModel>
#include <QFileInfo>
#include <QPushButton>
#include <QLineEdit>
#include <QHeaderView>
#include <utils/qtcassert.h>


namespace qook { namespace toolset {

SettingsWidget::SettingsWidget(const Core::Id &type_id, const QString & name)
    : ui(new Ui::SettingsWidget),
        selected_(0),
        type_id_(type_id),
        name_(name)
{
    ui->setupUi(this);

    model_ = new ConfigItemModel(type_id_, this);
    ui->overview->setModel(model_);

    ui->overview->header()->setSectionResizeMode(0, QHeaderView::Stretch);
    ui->overview->header()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    ui->overview->header()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    ui->overview->header()->setStretchLastSection(false);
    ui->overview->expandAll();

    ui->overview->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->overview->setSelectionBehavior(QAbstractItemView::SelectRows);
    connect(ui->overview->selectionModel(), &QItemSelectionModel::currentChanged, this, &SettingsWidget::current_selection_changed);
}

SettingsWidget::~SettingsWidget()
{
    delete ui;
}


void SettingsWidget::on_btnAdd_clicked()
{
    // add a new item
    QModelIndex indx = model_->add_tool(QString(), QFileInfo())->index();
    ui->overview->setCurrentIndex(indx);

}

void SettingsWidget::on_btnClone_clicked()
{
    QTC_ASSERT(selected_, return);

    QString clone_name = selected_->tool().display_name() + " (cloned)";
    QModelIndex indx = model_->add_tool(clone_name, selected_->tool().exec_file())->index();
    ui->overview->setCurrentIndex(indx);
}

void SettingsWidget::on_btnDelete_clicked()
{
    QTC_ASSERT(selected_, return);

    model_->remove_tool(selected_->tool().id());
    ui->overview->selectionModel()->clearSelection();
}

void SettingsWidget::on_btnDefault_clicked()
{
    QTC_ASSERT(selected_, return);
    model_->set_default_item_id(selected_->tool().id());
    ui->btnDefault->setEnabled(false);
}

void SettingsWidget::on_btnBrowse_clicked()
{
    QTC_ASSERT(selected_, return);

    QString file = QFileDialog::getOpenFileName(nullptr, QString("%1 executable").arg(name_), selected_->tool().exec_file().absoluteFilePath());
    if (!file.isNull())
    {
        selected_->tool().set_exec_file(QFileInfo(file));
        ui->txtVersion->setText(selected_->tool().version_string());
        ui->txtExec->setText(selected_->tool().user_file_name());
        selected_->update();
    }
}

void SettingsWidget::on_txtExec_textEdited(const QString & txt)
{
    QTC_ASSERT(selected_, return);

    // update the selected item
    selected_->tool().set_exec_file(QFileInfo(txt));
    ui->txtVersion->setText(selected_->tool().version_string());
    selected_->update();
}

void SettingsWidget::on_txtName_textEdited(const QString & txt)
{
    QTC_ASSERT(selected_, return);
    selected_->tool().set_display_name(txt);
    selected_->update();
}

void SettingsWidget::current_selection_changed(const QModelIndex & current, const QModelIndex & /*previous*/)
{
    selected_ = model_->find_item(current);

    bool has_information = selected_ != nullptr;
    bool is_editable = has_information && !selected_->tool().is_auto_detected();

    if (has_information)
    {
        const Tool & tool = selected_->tool();

        ui->txtName->setText(tool.display_name());
        ui->txtExec->setText(tool.exec_file().absoluteFilePath());
        ui->txtVersion->setText(tool.version_string());
    }
    else
    {
        ui->txtName->setText(QString());
        ui->txtExec->setText(QString());
        ui->txtVersion->setText(QString());
    }

    ui->txtName->setEnabled(is_editable);
    ui->txtExec->setEnabled(is_editable);
    ui->btnBrowse->setEnabled(is_editable);
    ui->btnDelete->setEnabled(is_editable);
    ui->btnClone->setEnabled(has_information);
    ui->btnDefault->setEnabled(has_information && !selected_->is_default());
}

} }




