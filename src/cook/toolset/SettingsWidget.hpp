#ifndef HEADER_cook_toolset_SettingsWidget_hpp_ALREADY_INCLUDED
#define HEADER_cook_toolset_SettingsWidget_hpp_ALREADY_INCLUDED

#include <QWidget>
#include <QFileInfo>
#include <QTreeWidgetItem>
#include <coreplugin/id.h>

namespace cook { namespace toolset {

class ConfigItemModel;
class ConfigItem;

namespace Ui {

class SettingsWidget;

}

class SettingsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SettingsWidget(const Core::Id & type_id, const QString & name);
    ~SettingsWidget();

    ConfigItemModel * model() { return model_; }

private slots:
    void on_btnAdd_clicked();
    void on_btnClone_clicked();
    void on_btnDelete_clicked();
    void on_btnDefault_clicked();
    void on_btnBrowse_clicked();

    void on_txtExec_textEdited(const QString & txt);
    void on_txtName_textEdited(const QString & txt);
    void current_selection_changed(const QModelIndex &current, const QModelIndex &previous);

private:
    Ui::SettingsWidget *ui;
    ConfigItemModel * model_;
    ConfigItem * selected_;
    Core::Id type_id_;
    QString name_;
};

} }

#endif
