#include "qook/QookProject.hpp"
#include "qook/Constants.hpp"

namespace qook {

QookProject::QookProject(const Utils::FileName &filename)
    : ProjectExplorer::Project(constants::CHAI_MIME_TYPE, filename)
{
    setId(constants::QOOK_PROJECT_ID);
    setDisplayName(projectDirectory().fileName());
}

QookProject::~QookProject()
{
}

bool QookProject::needsConfiguration() const
{
    return false;
}

bool QookProject::requiresTargetPanel() const
{
    return false;
}
bool QookProject::knowsAllBuildExecutables() const
{
    return true;
}

bool QookProject::supportsKit(ProjectExplorer::Kit * k, QString * errorMessage) const
{
    return true;
}

ProjectExplorer::ProjectImporter *QookProject::projectImporter() const
{
    return 0;
}

ProjectExplorer::Project::RestoreResult QookProject::fromMap(const QVariantMap &map, QString *errorMessage)
{
    return RestoreResult::Ok;
}

bool QookProject::setupTarget(ProjectExplorer::Target *t)
{
    return true;
}

QStringList QookProject::filesGeneratedFrom(const QString &sourceFile) const
{
    return QStringList();
}

} // namespace qook
