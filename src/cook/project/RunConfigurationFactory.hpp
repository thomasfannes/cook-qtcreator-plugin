#ifndef HEADER_cook_project_RunConfigurationFactory_hpp_ALREADY_INCLUDED
#define HEADER_cook_project_RunConfigurationFactory_hpp_ALREADY_INCLUDED

#include <projectexplorer/runconfiguration.h>
#include "cook/project/CookBuildTarget.hpp"

namespace cook { namespace project {

class RunConfigurationFactory : public ProjectExplorer::IRunConfigurationFactory
{
    Q_OBJECT

public:
    explicit RunConfigurationFactory(QObject *parent = 0);

    bool canCreate(ProjectExplorer::Target *parent, Core::Id id) const override;
    bool canRestore(ProjectExplorer::Target *parent, const QVariantMap &map) const override;
    bool canClone(ProjectExplorer::Target *parent, ProjectExplorer::RunConfiguration *product) const override;

    ProjectExplorer::RunConfiguration *clone(ProjectExplorer::Target *parent, ProjectExplorer::RunConfiguration *product) override;

    QList<Core::Id> availableCreationIds(ProjectExplorer::Target *parent, CreationMode mode) const override;
    QString displayNameForId(Core::Id id) const override;

private:
    bool can_handle(ProjectExplorer::Target *parent) const;

    ProjectExplorer::RunConfiguration *doCreate(ProjectExplorer::Target *parent, Core::Id id) override;
    ProjectExplorer::RunConfiguration *doRestore(ProjectExplorer::Target *parent, const QVariantMap &map) override;

    static info::Element unsafe_to_element_(const Core::Id & id);
    static std::pair<info::Element, bool> safe_to_element_(const Core::Id & id);
    const info::Recipe *find_recipe_(ProjectExplorer::Target *parent, const Core::Id & id) const;
};

} }

#endif
