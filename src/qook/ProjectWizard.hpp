#ifndef HEADER_qook_ProjectWizard_hpp_ALREADY_INCLUDED
#define HEADER_qook_ProjectWizard_hpp_ALREADY_INCLUDED

#include <coreplugin/basefilewizard.h>
#include <coreplugin/basefilewizardfactory.h>
#include <utils/filewizardpage.h>

namespace qook {

class ProjectWizardDialog :  public Core::BaseFileWizard
{
    Q_OBJECT

public:
    explicit ProjectWizardDialog(const Core::BaseFileWizardFactory * factory, QWidget * parent = 0);

    void setPath(const QString & path);


    Utils::FileWizardPage * first_page_;
};


class ProjectWizardFactory : public Core::BaseFileWizardFactory
{
    Q_OBJECT

public:
    ProjectWizardFactory();

protected:
    Core::BaseFileWizard *create(QWidget *parent, const Core::WizardDialogParameters &parameters) const override;
    Core::GeneratedFiles generateFiles(const QWizard *w, QString *errorMessage) const override;
    bool postGenerateFiles(const QWizard *w, const Core::GeneratedFiles &l, QString *errorMessage) const override;
};

} // namespace qook

#endif
