#include "cook/toolset/KitConfigWidget.hpp"
#include "cook/toolset/KitInformation.hpp"
#include "cook/toolset/Tool.hpp"
#include "cook/toolset/ToolManager.hpp"
#include "cook/Constants.hpp"

#include <utils/qtcassert.h>
#include <coreplugin/icore.h>

namespace cook { namespace toolset {

KitConfigWidget::KitConfigWidget(ProjectExplorer::Kit *kit, const ProjectExplorer::KitInformation *ki, const Core::Id &type_id, const QString &name)
    : ProjectExplorer::KitConfigWidget(kit, ki),
      combo_box_(new QComboBox),
      manage_btn_(new QPushButton(KitConfigWidget::msgManage())),
      type_id_(type_id),
      name_(name)
{
    ToolManager * mgr = ToolManager::instance();

    combo_box_->setEnabled(false);
    combo_box_->setToolTip(toolTip());

    foreach (const Tool * tool, mgr->registered_tools())
        if(tool->type_id() == type_id_)
            tool_added(tool->id());

    updateComboBox();

    refresh();
    connect(combo_box_, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &KitConfigWidget::current_tool_changed);

    manage_btn_->setContentsMargins(0, 0, 0, 0);
    connect(manage_btn_, &QPushButton::clicked, this, &KitConfigWidget::manage_tools);
    connect(mgr, &ToolManager::tool_added,      [this] (const Core::Id & id, const Core::Id & type_id) {  if(type_id == type_id_) KitConfigWidget::tool_added(id); });
    connect(mgr, &ToolManager::tool_removed,    [this] (const Core::Id & id, const Core::Id & type_id) {  if(type_id == type_id_) KitConfigWidget::tool_removed(id); });
    connect(mgr, &ToolManager::tool_updated,    [this] (const Core::Id & id, const Core::Id & type_id) {  if(type_id == type_id_) KitConfigWidget::tool_updated(id); });
}


KitConfigWidget::~KitConfigWidget()
{
    delete combo_box_;
    delete manage_btn_;
}


QString KitConfigWidget::displayName() const
{
    return QString("%1 Tool:").arg(name_);
}

void KitConfigWidget::makeReadOnly()
{
    combo_box_->setEnabled(false);
}


void KitConfigWidget::refresh()
{
    const Tool * tool = KitInformation::get_tool(type_id_, m_kit);
    combo_box_->setCurrentIndex(tool == 0 ? -1 : indexOf(tool->id()));
}


QWidget *KitConfigWidget::mainWidget() const
{
    return combo_box_;
}


QWidget *KitConfigWidget::buttonWidget() const
{
    return manage_btn_;
}


QString KitConfigWidget::toolTip() const
{
    return tr("The Cook Tool to use when building a project with Cook.<br>"
              "This setting is ignored when using other build systems.");
}


int KitConfigWidget::indexOf(const Core::Id &id)
{
    for (int i = 0; i < combo_box_->count(); ++i)
        if (id == Core::Id::fromSetting(combo_box_->itemData(i)))
            return i;

    return -1;
}


void KitConfigWidget::tool_added(const Core::Id &id)
{
    const Tool * tool = ToolManager::instance()->find_registered_tool(id);
    QTC_ASSERT(tool, return);

    combo_box_->addItem(tool->display_name(), tool->id().toSetting());
    updateComboBox();
    refresh();
}


void KitConfigWidget::tool_updated(const Core::Id &id)
{
    const Tool * tool = ToolManager::instance()->find_registered_tool(id);
    QTC_ASSERT(tool, return);

    const int pos = indexOf(id);
    QTC_ASSERT(pos >= 0, return);

    combo_box_->setItemText(pos, tool->display_name());
}


void KitConfigWidget::tool_removed(const Core::Id &id)
{
    const int pos = indexOf(id);
    QTC_ASSERT(pos >= 0, return);

    // do not handle the current index changed signal
    removing_item = true;
    combo_box_->removeItem(pos);
    removing_item = false;

    // update the checkbox and set the current index
    updateComboBox();
    refresh();
}


void KitConfigWidget::updateComboBox()
{
    // remove unavailable cmake tool:
    int pos = indexOf(Core::Id());

    if (pos >= 0)
        combo_box_->removeItem(pos);

    if (combo_box_->count() == 0)
    {
        combo_box_->addItem(tr("<No Cook tool available>"), Core::Id().toSetting());
        combo_box_->setEnabled(false);
    }
    else
    {
        combo_box_->setEnabled(true);
    }
}


void KitConfigWidget::current_tool_changed(int index)
{
    if (removing_item)
        return;

    const Core::Id id = Core::Id::fromSetting(combo_box_->itemData(index));
    KitInformation::set_cook_tool(m_kit, id);
}


void KitConfigWidget::manage_tools()
{
    Core::ICore::showOptionsDialog(constants::COOK_TOOLSET_SETTINGS_ID, buttonWidget());
}

} }

