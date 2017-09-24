#ifndef HEADER_qook_project_NinjaBuildStep_hpp_ALREADY_INCLUDED
#define HEADER_qook_project_NinjaBuildStep_hpp_ALREADY_INCLUDED

#include <projectexplorer/abstractprocessstep.h>

namespace qook { namespace project {

class NinjaBuildStep : public ProjectExplorer::AbstractProcessStep
{
public:
    explicit NinjaBuildStep(ProjectExplorer::BuildStepList *parent);

    virtual bool init(QList<const BuildStep *> &earlierSteps) override;
    virtual void run(QFutureInterface<bool> &fi) override;
    virtual bool fromMap(const QVariantMap &map) override;
    virtual QVariantMap toMap() const override;
    virtual ProjectExplorer::BuildStepConfigWidget *createConfigWidget() override;
    virtual void stdOutput(const QString &line) override;

private:
    friend class NinajBuildStepConfigWidget;
    friend class NinjaBuildStepFactory;

    NinjaBuildStep(ProjectExplorer::BuildStepList * bsl, Core::Id id);
    NinjaBuildStep(ProjectExplorer::BuildStepList *parent, NinjaBuildStep *product);

    QString all_arguments_() const;
    bool configure_process_parameters_(ProjectExplorer::ProcessParameters & param, bool needs_init);

    void ctor_();
    QString additional_arguments_;
    QRegExp percent_progress_;
    QRegExp ninja_progress_;
    QString ninja_progress_string_;
};

} }

#endif
