#ifndef HEADER_qook_QookProject_hpp_ALREADY_INCLUDED
#define HEADER_qook_QookProject_hpp_ALREADY_INCLUDED

#include <projectexplorer/project.h>

namespace qook {

class QookProject : public ProjectExplorer::Project
{
    Q_OBJECT

public:
    explicit QookProject(const Utils::FileName & filename);
    ~QookProject() final;

    bool needsConfiguration() const final;
    bool requiresTargetPanel() const final;
    bool knowsAllBuildExecutables() const final;

    bool supportsKit(ProjectExplorer::Kit * k, QString * errorMessage = 0) const final;
    ProjectExplorer::ProjectImporter *projectImporter() const final;

protected:
    RestoreResult fromMap(const QVariantMap &map, QString *errorMessage) final;
    bool setupTarget(ProjectExplorer::Target *t) final;

private:
    QStringList filesGeneratedFrom(const QString &sourceFile) const final;
};

}

#endif
