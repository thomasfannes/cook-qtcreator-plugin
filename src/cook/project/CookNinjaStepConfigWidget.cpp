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

    additional_arguments_ = new QLineEdit(this);
    fl->addRow(tr("Arguments:"), additional_arguments_);
    additional_arguments_->setText(cook_ninja_step_->additional_arguments());

    update_details_();

    Project * project = static_cast<Project*>(cook_ninja_step_->project());

    connect(additional_arguments_, &QLineEdit::textChanged, cook_ninja_step, &CookNinjaStep::set_additional_arguments);
    connect(cook_ninja_step, &CookNinjaStep::additional_arguments_changed,                this, &CookNinjaStepConfigWidget::update_details_);
    connect(project, &Project::activeProjectConfigurationChanged, this, &CookNinjaStepConfigWidget::update_details_);
    connect(project, &Project::activeProjectConfigurationChanged, this, &CookNinjaStepConfigWidget::update_details_);
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
    {
        summary_text_ = param.summary(displayName());
    }
    else
        summary_text_ = "<b>Build incorrectly configured</b>";

    emit updateSummary();
}



} }
