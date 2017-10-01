#ifndef HEADER_cook_toolset_Manager_hpp_ALREADY_INCLUDED
#define HEADER_cook_toolset_Manager_hpp_ALREADY_INCLUDED

#include "cook/toolset/Tool.hpp"
#include <utils/fileutils.h>
#include <coreplugin/id.h>
#include <QMap>

namespace Utils {

class PersistentSettingsWriter;

}

namespace cook { namespace toolset {

class Manager : public QObject
{
    Q_OBJECT

private:
    using Factory = ToolFactoryInterface;

    Manager(QObject * parent);

public:
    virtual ~Manager() override;

    static Manager * create_instance(QObject * parent);
    static Manager * instance();

    bool register_tool(Tool * tool);
    void deregister_tool(const Core::Id &id);
    const Tool * update_or_register_tool(const Tool & update);

    QList<const Tool *> registered_tools() const;
    const Tool * find_registered_tool(const Core::Id & id) const;

    void set_default_tool(const Core::Id &id);
    Core::Id default_tool_id(const Core::Id & type_id) const { return default_tools_.value(type_id, Core::Id()); }
    void restore();

    const ToolFactoryInterface * factory(const Core::Id & type_id) const;
    void register_factory(const Core::Id & type_id, ToolFactoryInterface * factory);

private:
    QList<Tool *> load_tools_();
    QList<Tool *> autodetect_tools_(const Factory &factory);
    void save_tools_();

signals:
    void default_tool_changed();
    void tool_added(const Core::Id & id, const Core::Id & type_id);
    void tool_removed(const Core::Id & id, const Core::Id & type_id);
    void tool_updated(const Core::Id & id, const Core::Id & type_id);
    void tools_changed();
    void tools_loaded();


private:
    Utils::FileName user_settings_filename();
    void initialize_writer_();

private:
    QList<Tool *> registered_tools_;
    Core::Id default_tool_id_;
    QString tool_name_;
    Utils::PersistentSettingsWriter * writer_;

    QMap<Core::Id, Factory *> factories_;
    QMap<Core::Id, Core::Id> default_tools_;

    static Manager * instance_;

};

} }

#endif
