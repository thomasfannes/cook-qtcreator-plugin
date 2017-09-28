//#include "qook/project/RunConfigurationFactory.hpp"
//#include "qook/project/BuildConfiguration.hpp"
//#include "qook/project/Project.hpp"
//#include <projectexplorer/target.h>

//namespace qook { namespace project {

//namespace {

//const char COOK_RC_PREFIX[] = "CookProjectManager.CookRunConfiguration.";

//}

//RunConfigurationFactory::RunConfigurationFactory(QObject *parent) :
//    ProjectExplorer::IRunConfigurationFactory(parent)
//{
//    setObjectName(QLatin1String("CookRunConfigurationFactory"));
//}

//// used to show the list of possible additons to a project, returns a list of ids
//QList<Core::Id> RunConfigurationFactory::availableCreationIds(ProjectExplorer::Target *parent, CreationMode mode) const
//{
//    Q_UNUSED(mode)
//    if (!canHandle(parent))
//        return QList<Core::Id>();

//    BuildConfiguration * bc = static_cast<BuildConfiguration *>(parent->activeBuildConfiguration());
//    QList<Core::Id> allIds;
//    for(const CookBuildTarget & tgt : bc->registered_targets())
//        allIds << idFromBuildTarget(tgt.uri);

//    return allIds;
//}

//// used to translate the ids to names to display to the user
//QString RunConfigurationFactory::displayNameForId(Core::Id id) const
//{
//    return buildTargetFromId(id);
//}

//bool RunConfigurationFactory::canHandle(ProjectExplorer::Target *parent) const
//{
//    if (!parent->project()->supportsKit(parent->kit()))
//        return false;
//    return qobject_cast<Project *>(parent->project());
//}

//bool RunConfigurationFactory::canCreate(ProjectExplorer::Target *parent, Core::Id id) const
//{
//    if (!canHandle(parent))
//        return false;
//    Project *project = static_cast<Project *>(parent->project());
//    return project->
//            hasBuildTarget(buildTargetFromId(id));
//}

////RunConfiguration *CMakeRunConfigurationFactory::doCreate(Target *parent, Core::Id id)
////{
////    CMakeProject *project = static_cast<CMakeProject *>(parent->project());
////    const QString title(buildTargetFromId(id));
////    const CMakeBuildTarget &ct = project->buildTargetForTitle(title);
////    return new CMakeRunConfiguration(parent, id, title, ct.workingDirectory, ct.title);
////}

////bool CMakeRunConfigurationFactory::canClone(Target *parent, RunConfiguration *source) const
////{
////    if (!canHandle(parent))
////        return false;
////    return source->id().name().startsWith(CMAKE_RC_PREFIX);
////}

////RunConfiguration *CMakeRunConfigurationFactory::clone(Target *parent, RunConfiguration * source)
////{
////    if (!canClone(parent, source))
////        return 0;
////    CMakeRunConfiguration *crc(static_cast<CMakeRunConfiguration *>(source));
////    return new CMakeRunConfiguration(parent, crc);
////}

////bool CMakeRunConfigurationFactory::canRestore(Target *parent, const QVariantMap &map) const
////{
////    if (!qobject_cast<CMakeProject *>(parent->project()))
////        return false;
////    return idFromMap(map).name().startsWith(CMAKE_RC_PREFIX);
////}

////RunConfiguration *CMakeRunConfigurationFactory::doRestore(Target *parent, const QVariantMap &map)
////{
////    const Core::Id id = idFromMap(map);
////    return new CMakeRunConfiguration(parent, id, buildTargetFromId(id), Utils::FileName(), QString());
////}

//QString RunConfigurationFactory::buildTargetFromId(Core::Id id)
//{
//    return id.suffixAfter(COOK_RC_PREFIX);
//}

//Core::Id RunConfigurationFactory::idFromBuildTarget(const QString &target)
//{
//    return Core::Id(COOK_RC_PREFIX).withSuffix(target);
//}


////} }
