#ifndef HEADER_cook_toolset_KitConfigWidget_hpp_ALREADY_INCLUDED
#define HEADER_cook_toolset_KitConfigWidget_hpp_ALREADY_INCLUDED

#include <projectexplorer/kitconfigwidget.h>
#include <QComboBox>
#include <QPushButton>

namespace cook { namespace toolset {

class KitConfigWidget : public ProjectExplorer::KitConfigWidget
{
public:
    KitConfigWidget(ProjectExplorer::Kit *kit, const ProjectExplorer::KitInformation *ki, const Core::Id & type_id, const QString & name);
    ~KitConfigWidget() override;

    // KitConfigWidget interface
    QString displayName() const override;
    void makeReadOnly() override;
    void refresh() override;
    QWidget * mainWidget() const override;
    QWidget * buttonWidget() const override;
    QString toolTip() const override;

private:
    int indexOf(const Core::Id &id);
    void updateComboBox();
    void tool_added(const Core::Id &id);
    void tool_updated(const Core::Id &id);
    void tool_removed(const Core::Id &id);
    void current_tool_changed(int index);
    void manage_tools();

    bool removing_item = false;
    QComboBox * combo_box_;
    QPushButton * manage_btn_;
    const Core::Id type_id_;
    const QString & name_;
};

} }

#endif
