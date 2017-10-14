#include "cook/project/CookNinjaStepConfigWidget.hpp"
#include "cook/project/CookNinjaStep.hpp"
#include "cook/project/Project.hpp"
#include "cook/toolset/KitInformation.hpp"
#include "cook/toolset/NinjaTool.hpp"

#include <projectexplorer/buildconfiguration.h>
#include <projectexplorer/target.h>
#include <projectexplorer/project.h>
#include <QFormLayout>

#include <cmakeprojectmanager/cmakebuildstep.h>

namespace cook { namespace project {

CookNinjaStepConfigWidget::CookNinjaStepConfigWidget(CookNinjaStep *cook_ninja_step) :
    cook_ninja_step_(cook_ninja_step),
    summary_text_()
{
    QFormLayout *fl = new QFormLayout(this);
    fl->setMargin(0);
    fl->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);
    setLayout(fl);

    update_details_();

    Project * project = static_cast<Project*>(cook_ninja_step_->project());

    connect(project, &Project::environmentChanged,      this, &CookNinjaStepConfigWidget::update_details_);
    connect(project, &Project::buildDirectoryChanged,   this, &CookNinjaStepConfigWidget::update_details_);
    connect(project, &Project::target_uri_changed,      this, &CookNinjaStepConfigWidget::update_details_);
}

QString CookNinjaStepConfigWidget::displayName() const
{
    return tr("Cook", "CookManager::CookNinjaStepConfigWidget display name.");
}

QString CookNinjaStepConfigWidget::summaryText() const
{
    return summary_text_;
}

void CookNinjaStepConfigWidget::update_details_()
{
    ProjectExplorer::ProcessParameters param;
    if (cook_ninja_step_->get_process_parameters_(param))
        summary_text_ = param.summary(displayName());
    else
        summary_text_ = "<b>Build correctly configured</b>";

    emit updateSummary();
}



} }
