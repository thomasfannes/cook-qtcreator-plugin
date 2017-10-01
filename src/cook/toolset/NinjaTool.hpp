#ifndef HEADER_cook_toolset_NinjaTool_hpp_ALREADY_INCLUDED
#define HEADER_cook_toolset_NinjaTool_hpp_ALREADY_INCLUDED

#include "cook/toolset/Tool.hpp"
#include <QFileInfo>
#include <utils/synchronousprocess.h>
#include <coreplugin/id.h>

namespace cook { namespace toolset {

class NinjaTool : public Tool
{
public:
    NinjaTool();
    NinjaTool(Detection d, const Core::Id & id);

    static Core::Id type_id() { return Core::Id("CookPlugin.ninja"); }

    bool is_default() const override;

private:
    std::pair<Tool::Version, bool> get_version_() const override;
};

class NinjaFactory : public ToolFactoryInterface
{
public:
    virtual Tool * construct() const                                        { return new NinjaTool(); }
    virtual Tool * construct(const Tool & rhs) const                        { return new NinjaTool(static_cast<const NinjaTool &>(rhs)); }
    virtual Tool * construct(Tool::Detection d, const Core::Id & id) const  { return new NinjaTool(d, id); }
    QString name() const                                                    { return "ninja"; }

    virtual QStringList executable_names() const { return {"ninja"}; }
};

} }

#endif
