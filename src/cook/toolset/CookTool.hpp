#ifndef HEADER_cook_toolset_CookTool_hpp_ALREADY_INCLUDED
#define HEADER_cook_toolset_CookTool_hpp_ALREADY_INCLUDED

#include "cook/toolset/Tool.hpp"
#include <QFileInfo>
#include <utils/synchronousprocess.h>
#include <coreplugin/id.h>

namespace cook { namespace toolset {

class CookTool : public Tool
{
public:
    CookTool();
    CookTool(Detection d, const Core::Id & id);

    static Core::Id type_id() { return Core::Id("CookPlugin.cook"); }

    bool is_default() const override;

private:
    std::pair<Tool::Version, bool> get_version_() const override;
};

class CookFactory : public ToolFactoryInterface
{
public:
    virtual Tool * construct() const                                        { return new CookTool(); }
    virtual Tool * construct(const Tool & rhs) const                        { return new CookTool(static_cast<const CookTool &>(rhs)); }
    virtual Tool * construct(Tool::Detection d, const Core::Id & id) const  { return new CookTool(d, id); }
    virtual QString name() const                                            { return "cook"; }

    virtual QStringList executable_names() const { return {"cook"}; }
};

} }

#endif
