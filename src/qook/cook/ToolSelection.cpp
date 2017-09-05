#include "ToolSelection.hpp"
#include "ui_ToolSelection.h"

#include <QFileDialog>
#include <QFileInfo>
#include <iostream>

namespace qook { namespace cook {

ToolSelection::ToolSelection(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ToolSelection),
    tool_(Tool())
{
    ui->setupUi(this);
}

ToolSelection::~ToolSelection()
{
    delete ui;
}

void ToolSelection::set_executable(const Tool & tool)
{
    tool_ = tool;
    ui->filename->setText(tool.filename());
    update_tool_ui_();
}

void ToolSelection::update_tool_ui_()
{
    const QString & txt = ui->filename->text();
    tool_.set_filename(txt);

    QFileInfo file(ui->filename->text());

    // set the color
    if (file.exists() && file.isFile() && tool_.is_valid())
    {
        ui->filename->setStyleSheet(QString());
        ui->isValid->setChecked(true);
        ui->version->setText(tool_.version_string());
    }
    else
    {
        ui->filename->setStyleSheet("QLineEdit { color : red; }");
        ui->isValid->setChecked(false);
        ui->version->setText(QString());
    }
}

void ToolSelection::on_browse_clicked()
{
    QString filename = QFileDialog::getOpenFileName(0, tr("Select Cook executable"), tool().filename(), QString());
    if (!filename.isEmpty())
    {
        tool_.set_filename(filename);
        ui->filename->setText(filename);

        update_tool_ui_();
    }
}

void ToolSelection::on_filename_textChanged(const QString & /*text*/)
{
    update_tool_ui_();
}


} }
