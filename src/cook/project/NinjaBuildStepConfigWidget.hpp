#ifndef HEADER_cook_project_NinjaBuildStepConfigWidget_hpp_ALREADY_INCLUDED
#define HEADER_cook_project_NinjaBuildStepConfigWidget_hpp_ALREADY_INCLUDED

#include <projectexplorer/buildstep.h>
#include <QLineEdit>

namespace cook { namespace project {

class NinjaBuildStep;

class NinjaBuildStepConfigWidget : public ProjectExplorer::BuildStepConfigWidget
{
    Q_OBJECT

public:
    NinjaBuildStepConfigWidget(NinjaBuildStep * ninja_build_step);

    QString displayName() const override;
    QString summaryText() const override;

private:
    void updateDetails();

    NinjaBuildStep *ninja_build_step_;
    QString summary_text_;
    QLineEdit *additional_arguments_;
};


} }

#endif
