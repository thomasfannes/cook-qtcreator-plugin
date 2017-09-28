//#ifndef HEADER_qook_project_RunConfigurationFactory_hpp_ALREADY_INCLUDED
//#define HEADER_qook_project_RunConfigurationFactory_hpp_ALREADY_INCLUDED

//#include <projectexplorer/runconfiguration.h>
//#include "qook/project/CookBuildTarget.hpp"

//namespace qook { namespace project {

//class RunConfigurationFactory : public ProjectExplorer::IRunConfigurationFactory
//{
//    Q_OBJECT

//public:
//    explicit RunConfigurationFactory(QObject *parent = 0);

//    bool canCreate(ProjectExplorer::Target *parent, Core::Id id) const override;
//    bool canRestore(ProjectExplorer::Target *parent, const QVariantMap &map) const override;
//    bool canClone(ProjectExplorer::Target *parent, ProjectExplorer::RunConfiguration *product) const override;
//    ProjectExplorer::RunConfiguration *clone(ProjectExplorer::Target *parent, ProjectExplorer::RunConfiguration *product) override;
//    QList<Core::Id> availableCreationIds(ProjectExplorer::Target *parent, CreationMode mode) const override;
//    QString displayNameForId(Core::Id id) const override;

//    static Core::Id idFromBuildTarget(const CookBuildTarget & target);
//    static QString buildTargetFromId(Core::Id id);

//private:
////    bool canHandle(ProjectExplorer::Target *parent) const;

//    ProjectExplorer::RunConfiguration *doCreate(ProjectExplorer::Target *parent, Core::Id id) override;
//    ProjectExplorer::RunConfiguration *doRestore(ProjectExplorer::Target *parent, const QVariantMap &map) override;
//};

//} }

//#endif
