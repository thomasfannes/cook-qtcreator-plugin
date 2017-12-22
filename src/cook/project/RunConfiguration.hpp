#ifndef HEADER_cook_project_RunConfiguration_hpp_ALREADY_INCLUDED
#define HEADER_cook_project_RunConfiguration_hpp_ALREADY_INCLUDED

#include <projectexplorer/runconfiguration.h>
#include "cook/project/CookBuildTarget.hpp"

namespace cook { namespace project {

class RunConfiguration : public ProjectExplorer::RunConfiguration
{
    Q_OBJECT

    friend class RunSettingsWidget;
    friend class ProjectExplorer::IRunConfigurationFactory;

public:
    ProjectExplorer::Runnable runnable() const override;
    QWidget *createConfigurationWidget() override;

    const CookBuildTarget & target() const { return target_; }

    QVariantMap toMap() const override;

protected:
    bool fromMap(const QVariantMap &map) override;

private:
    RunConfiguration(ProjectExplorer::Target * parent);

    void initialize(Core::Id id, const CookBuildTarget & target);
    void copyFrom(const ProjectExplorer::RunConfiguration *source);
    QString default_display_name_() const;

    CookBuildTarget target_;

};

} }

#endif
