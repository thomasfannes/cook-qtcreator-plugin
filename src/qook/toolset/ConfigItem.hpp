#ifndef HEADER_qook_toolset_ConfigItem_hpp_ALREADY_INCLUDED
#define HEADER_qook_toolset_ConfigItem_hpp_ALREADY_INCLUDED

#include <utils/treemodel.h>
#include "qook/toolset/Tool.hpp"

namespace qook { namespace toolset {

class ConfigItemModel;

class ConfigItem : public Utils::TreeItem
{
public:
    ConfigItem(const ToolFactoryInterface * factory);
    ConfigItem(const ToolFactoryInterface * factory, const Tool * tool);
    ~ConfigItem();

    virtual QVariant data(int column, int role) const override;

    Tool & tool()                   { return *tool_; }
    const Tool & tool() const       { return *tool_; }
    bool has_changed() const;

    bool is_default() const;

    void update_original_tool(const Tool * original_tool) { original_tool_ = original_tool; }


private:
    ConfigItemModel * model() const;
    Tool * tool_;
    const Tool * original_tool_;
    const ToolFactoryInterface * factory_;
};

} }

#endif
