#ifndef HEADER_qook_project_BuildConfigurationFactory_hpp_ALREADY_INCLUDED
#define HEADER_qook_project_BuildConfigurationFactory_hpp_ALREADY_INCLUDED

#include <projectexplorer/buildconfiguration.h>

namespace qook { namespace project {

class BuildConfigurationFactory : public ProjectExplorer::IBuildConfigurationFactory
{
    Q_OBJECT

public:
    explicit BuildConfigurationFactory(QObject *parent = nullptr);
    ~BuildConfigurationFactory();

    virtual int priority(const ProjectExplorer::Target *parent) const override;
    virtual QList<ProjectExplorer::BuildInfo *> availableBuilds(const ProjectExplorer::Target *parent) const override;
    virtual int priority(const ProjectExplorer::Kit *k, const QString &projectPath) const override;
    virtual QList<ProjectExplorer::BuildInfo *> availableSetups(const ProjectExplorer::Kit *k, const QString &projectPath) const override;
    virtual ProjectExplorer::BuildConfiguration *create(ProjectExplorer::Target *parent, const ProjectExplorer::BuildInfo *info) const override;

    virtual bool canClone(const ProjectExplorer::Target *parent, ProjectExplorer::BuildConfiguration *source) const override;
    virtual ProjectExplorer::BuildConfiguration *clone(ProjectExplorer::Target *parent, ProjectExplorer::BuildConfiguration *source) override;
    virtual bool canRestore(const ProjectExplorer::Target *parent, const QVariantMap &map) const override;
    virtual ProjectExplorer::BuildConfiguration *restore(ProjectExplorer::Target *parent, const QVariantMap &map) override;

private:
    bool can_handle_(const ProjectExplorer::Target * target) const;
    ProjectExplorer::BuildInfo * create_build_info_(const ProjectExplorer::Kit *k, const Utils::FileName &project_dir, ProjectExplorer::BuildConfiguration::BuildType build_type) const;

    static std::vector<ProjectExplorer::BuildConfiguration::BuildType> different_configurations_;
};



} // namespace prokect
} // namespace qook

#endif
