#ifndef HEADER_cook_project_CookNinjaStepConfigWidget_hpp_ALREADY_INCLUDED
#define HEADER_cook_project_CookNinjaStepConfigWidget_hpp_ALREADY_INCLUDED

#include <projectexplorer/buildstep.h>
#include <QLineEdit>

namespace cook { namespace project {

class CookNinjaStep;

class CookNinjaStepConfigWidget : public ProjectExplorer::BuildStepConfigWidget
{
    Q_OBJECT

public:
    CookNinjaStepConfigWidget(CookNinjaStep * cook_ninja_step);

    QString displayName() const override;
    QString summaryText() const override;

private:
    void update_details_();

    CookNinjaStep *cook_ninja_step_;
    QString summary_text_;
    QLineEdit * additional_arguments_;
};


} }

#endif
