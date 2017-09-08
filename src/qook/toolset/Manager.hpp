#ifndef HEADER_qook_toolset_Manager_hpp_ALREADY_INCLUDED
#define HEADER_qook_toolset_Manager_hpp_ALREADY_INCLUDED

#include <QObject>
#include <utils/fileutils.h>
#include <coreplugin/id.h>

namespace Utils {

class PersistentSettingsWriter;

}

namespace qook { namespace toolset {

class Tool;

class Manager : public QObject
{
    Q_OBJECT

private:
    Manager(QObject * parent);

public:
    virtual ~Manager() override;

    // static singleton methods
    static Manager * create_instance(QObject * parent);
    static Manager * instance();

    bool register_tool(Tool * tool);
    void deregister_tool(const Core::Id &id);
    const QList<Tool *> & registered_tools() const { return registered_tools_; }
    Tool * find_registered_tool(const Core::Id & id) const;

    void set_default_tool(const Core::Id &id);
    Core::Id default_tool_id() const { return default_tool_id_;  }

    void restore();

private:
    QList<Tool *> load_tools_();
    QList<Tool *> autodetect_tools_();

signals:
    void default_tool_changed();
    void tool_added(const Core::Id id);
    void tool_removed(const Core::Id id);
    void tools_changed();
    void tools_loaded();

private slots:
    void save_tools_();

private:
    static Manager * instance_;

    QList<Tool *> registered_tools_;
    Core::Id default_tool_id_;
    Utils::PersistentSettingsWriter * writer_;
};

} }

#endif
