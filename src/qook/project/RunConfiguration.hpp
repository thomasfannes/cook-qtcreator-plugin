#ifndef HEADER_qook_project_RunConfiguration_hpp_ALREADY_INCLUDED
#define HEADER_qook_project_RunConfiguration_hpp_ALREADY_INCLUDED

#include <projectexplorer/runconfiguration.h>
#include "qook/project/CookBuildStep.hpp"

namespace qook { namespace project {

class RunConfiguration : public ProjectExplorer::RunConfiguration
{
    Q_OBJECT

    friend class RunSettingsWidget;
    friend class RunConfigurationFactory;

public:
    RunConfiguration(ProjectExplorer::Target * parent, Core::Id id, const CookBuildTarget & target);

    ProjectExplorer::Runnable runnable() const override;
    QWidget *createConfigurationWidget() override;

//    void setExecutable(const QString &executable);
//    void setBaseWorkingDirectory(const Utils::FileName &workingDirectory);

//    QVariantMap toMap() const override;

//    QString disabledReason() const override;

//    QString buildSystemTarget() const final;

//protected:
//    RunConfiguration(ProjectExplorer::Target *parent, RunConfiguration *source);
//    bool fromMap(const QVariantMap &map) override;

//    void updateEnabledState() final;

private:
    QString default_display_name_() const;

    RunConfiguration(ProjectExplorer::Target *parent, RunConfiguration * source);

    CookBuildTarget target_;

//    QString baseWorkingDirectory() const;
    void ctor();

//    CookBuildTarget build_system_target_;
//    QString executable_;
//    QString title_;
};

} }

#endif
