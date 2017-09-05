#ifndef HEADER_qook_plugin_Plugin_hpp_ALREADY_INCLUDED
#define HEADER_qook_plugin_Plugin_hpp_ALREADY_INCLUDED

#include <extensionsystem/iplugin.h>

namespace qook { namespace plugin {

class Plugin : public ExtensionSystem::IPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QtCreatorPlugin" FILE "QookPlugin.json")

public:
    Plugin();
    ~Plugin();

    virtual bool initialize(const QStringList &arguments, QString *errorString) override;
    virtual void extensionsInitialized() override;

private:
    void triggerAction();
};

} }

#endif
