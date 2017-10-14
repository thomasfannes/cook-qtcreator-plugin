#ifndef HEADER_cook_project_NinjaBuildStep_hpp_ALREADY_INCLUDED
#define HEADER_cook_project_NinjaBuildStep_hpp_ALREADY_INCLUDED

#include <projectexplorer/abstractprocessstep.h>

namespace cook { namespace project {

class NinjaBuildStep : public ProjectExplorer::AbstractProcessStep
{
    Q_OBJECT

    friend class NinjaBuildStepFactory;
    friend class NinjaBuildStepConfigWidget;

public:
    explicit NinjaBuildStep(ProjectExplorer::BuildStepList *bsl);

    virtual bool init(QList<const BuildStep *> &earlierSteps) override;
    virtual void run(QFutureInterface<bool> &interface) override;
    virtual ProjectExplorer::BuildStepConfigWidget *createConfigWidget() override;
    virtual bool immutable() const override;
    virtual QVariantMap toMap() const override;
    virtual void stdOutput(const QString &line) override;

    void set_clean(bool clean) { clean_ = clean; }
    void set_additional_arguments(const QString &list);
    void add_build_target(const QString &target, bool on);
    QString additional_arguments() const;

signals:
    void additional_arguments_changed(const QString &);

protected:
    NinjaBuildStep(ProjectExplorer::BuildStepList *bsl, NinjaBuildStep *bs);
    NinjaBuildStep(ProjectExplorer::BuildStepList *bsl, Core::Id id);

    bool fromMap(const QVariantMap &map) override;
    Utils::FileName ninja_build_file() const;

private:
    QString process_arguments_() const;
    bool configure_process_parameters_(ProjectExplorer::ProcessParameters & param, bool needs_init);
    void ctor_();

    struct NinjaSettings
    {
        NinjaSettings();

        QRegExp progress;
        QString progress_string;
    };
    const NinjaSettings ninja_settings_;

    QStringList build_targets_;
    QString additional_arguments_;
    bool clean_ = false;
};


} }

#endif
