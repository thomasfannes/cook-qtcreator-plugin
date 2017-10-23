#ifndef HEADER_cook_plugin_Plugin_hpp_ALREADY_INCLUDED
#define HEADER_cook_plugin_Plugin_hpp_ALREADY_INCLUDED

#include <extensionsystem/iplugin.h>
#include <utils/parameteraction.h>

namespace cook { namespace plugin {

class Plugin : public ExtensionSystem::IPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QtCreatorPlugin" FILE "CookProjectManager.json")

public:
    Plugin();
    ~Plugin();

    virtual bool initialize(const QStringList &arguments, QString *errorString) override;
    virtual void extensionsInitialized() override;

private:
    void updateContextActions();

    Utils::ParameterAction * m_buildTargetContextAction;
};

} }

#endif
