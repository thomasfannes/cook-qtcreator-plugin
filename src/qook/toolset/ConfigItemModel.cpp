#include "qook/toolset/ConfigItemModel.hpp"
#include "qook/toolset/ConfigItem.hpp"
#include "qook/toolset/Manager.hpp"
#include "qook/toolset/Tool.hpp"
#include <QUuid>
#include <utils/qtcassert.h>

namespace qook { namespace toolset {

ConfigItemModel::ConfigItemModel(QObject *parent, Manager * mgr)
    : Utils::TreeModel<Utils::TreeItem, Utils::TreeItem, ConfigItem>(parent),
      mgr_(mgr),
      default_item_id_(mgr->default_tool_id())
{
    setHeader({ tr("Name"), tr("Location"), tr("Version") });
    rootItem()->appendChild(new Utils::StaticTreeItem(tr("Auto-detected")));
    rootItem()->appendChild(new Utils::StaticTreeItem(tr("Manual")));

    // add the existing item
    for(Tool * tool: mgr->registered_tools())
        add_tool(tool);

    // connect the signals
    connect(mgr, &Manager::tool_removed, this, &ConfigItemModel::remove_tool);
    connect(mgr, &Manager::tool_added, this, [this](const Core::Id & id) { add_tool(mgr_->find_registered_tool(id)); });
    connect(mgr, &Manager::default_tool_changed, this, [this]() { set_default_item_id(mgr_->default_tool_id()); });
}

ConfigItem * ConfigItemModel::add_tool(Tool * tool)
{
    QTC_ASSERT(tool, return nullptr);

    // already an item with this id?
    ConfigItem * item = find_item(tool->id());
    if(item)
        return item;

    item = new ConfigItem(tool);
    (tool->is_auto_detected() ? auto_group_item() : manual_group_item())->appendChild(item);

    return item;
}

ConfigItem * ConfigItemModel::add_tool(const QString & displayName, const QFileInfo & executable)
{
    ConfigItem * item = new ConfigItem();
    item->tool().set_display_name(displayName);
    item->tool().set_exec_file(executable);
    manual_group_item()->appendChild(item);

    return item;
}

void ConfigItemModel::update_tool(const Core::Id &id, const QString &display_name, const QFileInfo &executable)
{
    ConfigItem * item = find_item(id);
    if (item == nullptr)
        return;

    item->tool().set_display_name(display_name);
    item->tool().set_exec_file(executable);
    item->reevaluate_changed(*mgr_);
}

void ConfigItemModel::remove_tool(const Core::Id & id)
{
    ConfigItem * item = find_item(id);
    if( item == nullptr)
        return;

    // add to be removed on apply
    to_remove_.append(id);

    // remove from the model
    destroyItem(item);
}

ConfigItem * ConfigItemModel::find_item(const Core::Id & id) const
{
    return findItemAtLevel<2>( [id](ConfigItem * item){ return item->tool().id() == id; });
}

ConfigItem * ConfigItemModel::find_item(const QModelIndex & index) const
{
    return itemForIndexAtLevel<2>(index);
}

void ConfigItemModel::apply()
{
    QList<ConfigItem*> to_update;

    // apply all the changes to the manager
    forItemsAtLevel<2>( [this, &to_update](ConfigItem * item)
    {
        if (item->has_changed())
        {
            // new item?
            Tool * tool = mgr_->find_registered_tool(item->tool().id());
            if (!tool)
            {
                tool = new Tool(item->tool());
                mgr_->register_tool(tool);
                item->update_original_tool(tool);
            }
            else
            {
                tool->set_display_name(item->tool().display_name());
                tool->set_exec_file(item->tool().exec_file());
            }

            to_update << item;
        }
    });

    // remove the tools that need to be removed
    for(const Core::Id & id : to_remove_)
        mgr_->deregister_tool(id);
    to_remove_.clear();

    // update the default item (if changed)
    if(default_item_id() != mgr_->default_tool_id())
        mgr_->set_default_tool(default_item_id());

    for(ConfigItem * item : to_update)
    {
        item->reevaluate_changed(*mgr_);
        item->update();
    }
}

Utils::TreeItem * ConfigItemModel::auto_group_item() const
{
    return rootItem()->childAt(0);
}

Utils::TreeItem * ConfigItemModel::manual_group_item() const
{
    return rootItem()->childAt(1);
}

Core::Id ConfigItemModel::default_item_id() const
{
    return default_item_id_;
}

void ConfigItemModel::update_(const Core::Id &id)
{
    ConfigItem * item = find_item(id);
    if (item)
        item->reevaluate_changed(*mgr_);
}

void ConfigItemModel::set_default_item_id(const Core::Id &id)
{
    if (id == default_item_id_)
        return;

    Core::Id old_id = default_item_id_;
    default_item_id_ = id;

    update_(old_id);
    update_(id);
}

} }


