#ifndef HEADER_qook_cook_ToolManager_hpp_ALREADY_INCLUDED
#define HEADER_qook_cook_ToolManager_hpp_ALREADY_INCLUDED

#include "qook/cook/Tool.hpp"
#include <QObject>
#include <utils/fileutils.h>

namespace qook { namespace cook {

class Tool;

class ToolManager : public QObject
{
private:
    ToolManager(QObject * parent);

public:
    virtual ~ToolManager() override;

    // static singleton methods
    static ToolManager * create_instance(QObject * parent);
    static ToolManager * instance();

    const QList<Tool *> & registered_tools() const { return registered_tools_; }
    Tool * default_tool() const { return default_tool_; }

    Core::Id register_or_find_tool(const Utils::FileName & command) const;
    void deregister_tool(const Core::Id & id) const;
    set_default_tool(const Core::Id & id) const;

    Tool *find_by_command(const Utils::FileName &command) const;
    Tool *find_by_id(const Core::Id &id) const;

signals:
    void default_tool_changed();
    void tool_added(const Core::Id id);
    void tool_removed(const Core::Id id);
    void tools_changed();
    void tools_loaded();




private:
    static ToolManager * instance_;

    QList<Tool *> registered_tools_;
    Tool * default_tool_;
};

} }

#endif
