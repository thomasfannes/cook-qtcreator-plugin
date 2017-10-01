#include "cook/project/NinjaBuildStepConfigWidget.hpp"
#include "cook/project/NinjaBuildStep.hpp"
#include "cook/toolset/KitInformation.hpp"
#include "cook/toolset/NinjaTool.hpp"
#include <projectexplorer/buildconfiguration.h>
#include <projectexplorer/target.h>
#include <projectexplorer/project.h>
#include <QFormLayout>

namespace cook { namespace project {

NinjaBuildStepConfigWidget::NinjaBuildStepConfigWidget(NinjaBuildStep *ninja_build_step) :
    ninja_build_step_(ninja_build_step),
    summary_text_(),
    additional_arguments_(0)
{
    QFormLayout *fl = new QFormLayout(this);
    fl->setMargin(0);
    fl->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);
    setLayout(fl);

    additional_arguments_ = new QLineEdit(this);
    fl->addRow(tr("Arguments:"), additional_arguments_);
    additional_arguments_->setText(ninja_build_step_->additional_arguments());

    updateDetails();

    connect(additional_arguments_, &QLineEdit::textChanged, ninja_build_step, &NinjaBuildStep::set_additional_arguments);
    connect(ninja_build_step, &NinjaBuildStep::additional_arguments_changed, this, &NinjaBuildStepConfigWidget::updateDetails);
    ninja_build_step_->project()->subscribeSignal(&ProjectExplorer::BuildConfiguration::environmentChanged, this, [this]()
    {
        if (static_cast<ProjectExplorer::BuildConfiguration *>(sender())->isActive())
            updateDetails();
    });

    connect(ninja_build_step->project(), &ProjectExplorer::Project::activeProjectConfigurationChanged,
            this, [this](ProjectExplorer::ProjectConfiguration *pc)
    {
        if (pc->isActive())
            updateDetails();
    });
}

QString NinjaBuildStepConfigWidget::displayName() const
{
    return tr("Ninja", "CookManager::NinjaBuildStepConfigWidget display name.");
}

QString NinjaBuildStepConfigWidget::summaryText() const
{
    return summary_text_;
}

void NinjaBuildStepConfigWidget::updateDetails()
{
    ProjectExplorer::BuildConfiguration *bc = ninja_build_step_->buildConfiguration();
    if (!bc)
        bc = ninja_build_step_->target()->activeBuildConfiguration();

    const toolset::NinjaTool * tool = toolset::KitInformation::ninja_tool(ninja_build_step_->target()->kit());
    if(tool)
    {
        ProjectExplorer::ProcessParameters param;
        param.setMacroExpander(ninja_build_step_->macroExpander());
        param.setEnvironment(bc->environment());
        param.setWorkingDirectory(bc->buildDirectory().toString());
        param.setCommand(tool->user_file_name());
        param.setArguments(ninja_build_step_->process_arguments_());
        summary_text_ = param.summary(displayName());
    }
    else
    {
        summary_text_ = "<b>No Ninja tool set</b>";
    }

    emit updateSummary();
}



} }
