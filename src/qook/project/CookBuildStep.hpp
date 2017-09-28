#ifndef HEADER_qook_project_CookBuildStep_hpp_ALREADY_INCLUDED
#define HEADER_qook_project_CookBuildStep_hpp_ALREADY_INCLUDED

#include "qook/project/CookBuildTarget.hpp"
#include <projectexplorer/abstractprocessstep.h>

namespace qook { namespace project {

class BuildConfiguration;
class RunConfiguration;
class Project;
class CookBuildStepConfigWidget;

class CookBuildStep : public ProjectExplorer::AbstractProcessStep
{
    Q_OBJECT
public:
    explicit CookBuildStep(ProjectExplorer::BuildStepList *parent);

    virtual bool init(QList<const BuildStep *> &earlierSteps) override;
    virtual void run(QFutureInterface<bool> &fi) override;
    virtual bool fromMap(const QVariantMap &map) override;
    virtual QVariantMap toMap() const override;
    virtual ProjectExplorer::BuildStepConfigWidget *createConfigWidget() override;

    QList<CookBuildTarget> all_targets() const;
    bool builds_target(const CookBuildTarget & target) const;

signals:
    void build_targets_changed();
    void build_target_changed();

private:
    Project * project_() const;

    friend class CookBuildStepConfigWidget;
    friend class CookBuildStepFactory;

    CookBuildStep(ProjectExplorer::BuildStepList * bsl, Core::Id id);
    CookBuildStep(ProjectExplorer::BuildStepList *parent, CookBuildStep *product);
    void ctor_(ProjectExplorer::BuildStepList * parent);
    BuildConfiguration * build_configuration_() const;
    RunConfiguration * run_configuration_() const;
    void set_build_target_(const CookBuildTarget & target);
    bool configure_process_parameters_(ProjectExplorer::ProcessParameters & param, RunConfiguration * run_configuration, bool needs_init);
    QString all_arguments_(RunConfiguration * run_configuration) const;

    CookBuildTarget build_target_;
};


} }



#endif
