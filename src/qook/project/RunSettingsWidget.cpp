//#include "qook/project/RunSettingsWidget.hpp"
//#include "qook/project/RunConfiguration.hpp"
//#include <QFormLayout>
//#include <QLabel>
//#include <projectexplorer/runnables.h>
//#include <projectexplorer/runconfigurationaspects.h>
//#include <utils/fancylineedit.h>
//#include <utils/detailswidget.h>

//namespace qook { namespace project {

//namespace {

//static void updateExecutable(RunConfiguration *rc, Utils::FancyLineEdit *fle)
//{
//    const ProjectExplorer::Runnable runnable = rc->runnable();
//    fle->setText(runnable.is<ProjectExplorer::StandardRunnable>()
//                 ? Utils::FileName::fromString(runnable.as<ProjectExplorer::StandardRunnable>().executable).toUserOutput()
//                 : QString());
//}

//}

//RunSettingsWidget::RunSettingsWidget(RunConfiguration * run_configuration, QWidget * parent)
//    : QWidget(parent)
//{
//    auto fl = new QFormLayout();
//    fl->setMargin(0);
//    fl->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);

//    auto executableLabel = new QLabel(tr("Executable:"));
//    auto executable = new Utils::FancyLineEdit;
//    executable->setReadOnly(true);
//    executable->setPlaceholderText(tr("<unknown>"));
//    connect(run_configuration, &RunConfiguration::enabledChanged, this, std::bind(updateExecutable, run_configuration, executable));
//    updateExecutable(run_configuration, executable);

//    fl->addRow(executableLabel, executable);

//    run_configuration->extraAspect<ProjectExplorer::ArgumentsAspect>()->addToMainConfigurationWidget(this, fl);
//    run_configuration->extraAspect<ProjectExplorer::WorkingDirectoryAspect>()->addToMainConfigurationWidget(this, fl);
//    run_configuration->extraAspect<ProjectExplorer::TerminalAspect>()->addToMainConfigurationWidget(this, fl);

//    auto detailsContainer = new Utils::DetailsWidget(this);
//    detailsContainer->setState(Utils::DetailsWidget::NoSummary);

//    auto detailsWidget = new QWidget(detailsContainer);
//    detailsContainer->setWidget(detailsWidget);
//    detailsWidget->setLayout(fl);

//    auto vbx = new QVBoxLayout(this);
//    vbx->setMargin(0);
//    vbx->addWidget(detailsContainer);
//}


//} }
