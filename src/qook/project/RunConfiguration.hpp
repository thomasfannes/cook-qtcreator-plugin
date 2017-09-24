#ifndef HEADER_qook_project_RunConfiguration_hpp_ALREADY_INCLUDED
#define HEADER_qook_project_RunConfiguration_hpp_ALREADY_INCLUDED

#include <projectexplorer/runconfiguration.h>
#include "qook/project/CookBuildStep.hpp"

namespace qook { namespace project {

class RunConfiguration : public ProjectExplorer::RunConfiguration
{
    Q_OBJECT
    friend class CMakeRunConfigurationWidget;
    friend class CMakeRunConfigurationFactory;

public:
    RunConfiguration(ProjectExplorer::Target * parent, Core::Id id, const CookBuildTarget & target, const Utils::FileName &workingDirectory, const QString &title);

    ProjectExplorer::Runnable runnable() const override;
    QWidget *createConfigurationWidget() override;

    void setExecutable(const QString &executable);
    void setBaseWorkingDirectory(const Utils::FileName &workingDirectory);

    QString title() const;

    QVariantMap toMap() const override;

    QString disabledReason() const override;

    QString buildSystemTarget() const final;

protected:
    RunConfiguration(ProjectExplorer::Target *parent, RunConfiguration *source);
    bool fromMap(const QVariantMap &map) override;
    QString defaultDisplayName() const;

    void updateEnabledState() final;

private:
    QString baseWorkingDirectory() const;
    void ctor();

    CookBuildTarget build_system_target_;
    QString executable_;
    QString title_;
};

} }

#endif
