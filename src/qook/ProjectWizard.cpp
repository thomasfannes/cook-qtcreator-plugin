#include "qook/ProjectWizard.hpp"
#include "qook/Constants.hpp"
#include <projectexplorer/projectexplorer.h>

namespace qook {

ProjectWizardDialog::ProjectWizardDialog(const Core::BaseFileWizardFactory * factory, QWidget * parent)
    : Core::BaseFileWizard(factory, QVariantMap(), parent)
{
    setWindowTitle(tr("Create new Cook project"));

    // first page
    first_page_ = new Utils::FileWizardPage();
    first_page_->setTitle(tr("Project Name and Location"));
    first_page_->setFileNameLabel(tr("Project name:"));
    first_page_->setPathLabel(tr("Location:"));
    addPage(first_page_);

}

void ProjectWizardDialog::setPath(const QString & path)
{
    first_page_->setPath(path);
}

ProjectWizardFactory::ProjectWizardFactory()
{
    setSupportedProjectTypes( { constants::QOOK_PROJECT_ID});
//    setIcon(QIcon(QLatin1String(":/genericprojectmanager/images/genericprojectmanager.png")));
    setDisplayName(tr("Create a new Cook Project"));
    setId("QookPlugin.project");
    setDescription(tr("Creates a new Cook project."));
    setCategory(QLatin1String(ProjectExplorer::Constants::IMPORT_WIZARD_CATEGORY));
    setDisplayCategory(QLatin1String(ProjectExplorer::Constants::IMPORT_WIZARD_CATEGORY_DISPLAY));
    setFlags(Core::IWizardFactory::PlatformIndependent);
}

Core::BaseFileWizard * ProjectWizardFactory::create(QWidget *parent, const Core::WizardDialogParameters &parameters) const
{
    ProjectWizardDialog * wizard = new ProjectWizardDialog(this, parent);

    wizard->setPath(parameters.defaultPath());
    for(QWizardPage * p : wizard->extensionPages())
        wizard->addPage(p);

    return wizard;
}

Core::GeneratedFiles ProjectWizardFactory::generateFiles(const QWizard *w, QString *errorMessage) const
{
    return Core::GeneratedFiles();
}

bool ProjectWizardFactory::postGenerateFiles(const QWizard *w, const Core::GeneratedFiles &l, QString *errorMessage) const
{
    return true;
}

} // namespace qook
