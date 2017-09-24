#ifndef HEADER_qook_project_NinjaBuildStepConfigWidget_hpp_ALREADY_INCLUDED
#define HEADER_qook_project_NinjaBuildStepConfigWidget_hpp_ALREADY_INCLUDED

#include <projectexplorer/buildstep.h>

namespace qook { namespace project {

class NinjaBuildStep;

class NinjaBuildStepConfigWidget : public ProjectExplorer::BuildStepConfigWidget
{
public:
    explicit NinjaBuildStepConfigWidget(NinjaBuildStep * build_step);

    virtual QString summaryText() const;
    virtual QString displayName() const;

private:
    NinjaBuildStep * build_step_;
};

} }

#endif
