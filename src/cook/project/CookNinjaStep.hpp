#ifndef HEADER_cook_project_CookNinjaStep_hpp_ALREADY_INCLUDED
#define HEADER_cook_project_CookNinjaStep_hpp_ALREADY_INCLUDED

#include <projectexplorer/abstractprocessstep.h>
#include <QDebug>

namespace cook { namespace project {

class CookNinjaStep : public ProjectExplorer::AbstractProcessStep
{
    Q_OBJECT

    friend class CookNinjaStepFactory;
    friend class CookNinjaStepConfigWidget;

public:
    explicit CookNinjaStep(ProjectExplorer::BuildStepList *bsl);

    virtual bool init(QList<const BuildStep *> & earlierSteps) override;
    virtual void run(QFutureInterface<bool> & interface) override;
    virtual ProjectExplorer::BuildStepConfigWidget *createConfigWidget() override;

    void set_clean(bool clean) { clean_ = clean; }
    void set_additional_arguments(const QString & arguments);
    QString additional_arguments() const;

signals:
    void additional_arguments_changed(const QString & args);

protected:
    CookNinjaStep(ProjectExplorer::BuildStepList *bsl, CookNinjaStep *bs);
    CookNinjaStep(ProjectExplorer::BuildStepList *bsl, Core::Id id);

private:
    bool get_process_parameters_(ProjectExplorer::ProcessParameters & pp);
    QStringList addition_arguments_lists();
    void ctor_();
    bool clean_ = false;
    QString additional_arguments_;
};

} }

#endif

