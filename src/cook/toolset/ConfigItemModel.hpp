#ifndef HEADER_cook_toolset_ConfigItemModel_hpp_ALREADY_INCLUDED
#define HEADER_cook_toolset_ConfigItemModel_hpp_ALREADY_INCLUDED

#include <coreplugin/id.h>
#include <utils/treemodel.h>
#include <QFileInfo>

namespace cook { namespace toolset {

class Tool;
class ConfigItem;
class ToolFactoryInterface;

class ConfigItemModel : public Utils::TreeModel<Utils::TreeItem, Utils::TreeItem, ConfigItem>
{
public:
    explicit ConfigItemModel(const Core::Id & type_id, QObject * parent);

    ConfigItem * add_tool(const Tool *tool);
    ConfigItem * add_tool(const QString & displayName, const QFileInfo & executable);

    void update_tool(const Core::Id &id, const QString &display_name, const QFileInfo &executable);
    void remove_tool(const Core::Id & id);
    void apply();

    Utils::TreeItem * auto_group_item() const;
    Utils::TreeItem * manual_group_item() const;

    Core::Id default_item_id() const;
    void set_default_item_id(const Core::Id & id);

    ConfigItem * find_item(const Core::Id & id) const;
    ConfigItem * find_item(const QModelIndex & index) const;

private:
    void update_(const Core::Id & id);

    Core::Id default_item_id_;
    QList<Core::Id> to_remove_;
    const Core::Id type_id_;
    const ToolFactoryInterface * factory_;
};

} }

#endif
