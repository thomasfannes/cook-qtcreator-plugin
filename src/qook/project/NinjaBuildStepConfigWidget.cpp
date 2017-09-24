#include "qook/project/NinjaBuildStepConfigWidget.hpp"

namespace qook { namespace project {

NinjaBuildStepConfigWidget::NinjaBuildStepConfigWidget(NinjaBuildStep * build_step)
    : build_step_(build_step)
{
}


QString NinjaBuildStepConfigWidget::summaryText() const
{
    return "ninja";
}

QString NinjaBuildStepConfigWidget::displayName() const
{
    return "NinjaBuildStepConfig";
}


} }
