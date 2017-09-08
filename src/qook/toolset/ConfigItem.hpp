#ifndef HEADER_qook_toolset_ConfigItem_hpp_ALREADY_INCLUDED
#define HEADER_qook_toolset_ConfigItem_hpp_ALREADY_INCLUDED

#include <utils/treemodel.h>
#include "qook/toolset/Tool.hpp"

namespace qook { namespace toolset {

class ConfigItemModel;
class Manager;

class ConfigItem : public Utils::TreeItem
{
public:
    ConfigItem();
    ConfigItem(const Tool *tool);

    virtual QVariant data(int column, int role) const override;

    Tool & tool()               { return tool_; }
    const Tool & tool() const   { return tool_; }
    bool has_changed() const    { return changed_; }
    void reevaluate_changed(const Manager & tool_manager);
    bool is_default() const;

    void update_original_tool(const Tool * original_tool) { original_tool_ = original_tool; }


private:
    ConfigItemModel * model() const;
    Tool tool_;
    const Tool * original_tool_;
    bool changed_;
};

} }

#endif
