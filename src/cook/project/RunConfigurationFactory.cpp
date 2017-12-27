#include "cook/project/RunConfigurationFactory.hpp"
#include "cook/project/RunConfiguration.hpp"
#include "cook/project/BuildConfiguration.hpp"
#include "cook/project/Project.hpp"
#include <projectexplorer/target.h>

namespace cook { namespace project {


RunConfigurationFactory::RunConfigurationFactory(QObject *parent) :
    ProjectExplorer::IRunConfigurationFactory(parent)
{
    setObjectName(QLatin1String("CookRunConfigurationFactory"));
}

bool RunConfigurationFactory::canCreate(ProjectExplorer::Target *parent, Core::Id id) const
{
    return can_handle(parent) && find_recipe_(parent, id);
}

bool RunConfigurationFactory::canClone(ProjectExplorer::Target *parent, ProjectExplorer::RunConfiguration *source) const
{
    return can_handle(parent) && qobject_cast<RunConfiguration*>(source) && find_recipe_(parent, source->id());
}


bool RunConfigurationFactory::canRestore(ProjectExplorer::Target *parent, const QVariantMap &map) const
{
    if (!qobject_cast<Project *>(parent->project()) && !qobject_cast<BuildConfiguration *>(parent->activeBuildConfiguration()))
        return false;

    return safe_to_element_(ProjectExplorer::idFromMap(map)).second;
}

ProjectExplorer::RunConfiguration * RunConfigurationFactory::clone(ProjectExplorer::Target *parent, ProjectExplorer::RunConfiguration * source)
{
    if (!canClone(parent, source))
        return 0;

    return ProjectExplorer::IRunConfigurationFactory::cloneHelper<RunConfiguration>(parent, source);
}

QString RunConfigurationFactory::displayNameForId(Core::Id id) const
{
    return info::long_display_name(unsafe_to_element_(id));
}

QList<Core::Id> RunConfigurationFactory::availableCreationIds(ProjectExplorer::Target *parent, CreationMode mode) const
{
    Q_UNUSED(mode)
    if (!can_handle(parent))
        return QList<Core::Id>();

    BuildConfiguration * bc = static_cast<BuildConfiguration *>(parent->activeBuildConfiguration());
    QList<Core::Id> allIds;

    for(const info::Element & recipe : bc->all_targets())
        allIds << recipe.to_id();

   return allIds;
}

bool RunConfigurationFactory::can_handle(ProjectExplorer::Target *parent) const
{
    if (!parent->project()->supportsKit(parent->kit()))
        return false;

    return qobject_cast<BuildConfiguration*>(parent->activeBuildConfiguration());
}

ProjectExplorer::RunConfiguration * RunConfigurationFactory::doCreate(ProjectExplorer::Target *parent, Core::Id id)
{
    const info::Recipe * recipe = find_recipe_(parent, id);
    if(!recipe)
        return nullptr;

    return ProjectExplorer::IRunConfigurationFactory::createHelper<RunConfiguration>(parent, recipe->to_id(), CookBuildTarget(*recipe));
}

ProjectExplorer::RunConfiguration * RunConfigurationFactory::doRestore(ProjectExplorer::Target *parent, const QVariantMap &map)
{
    info::Element element = unsafe_to_element_(ProjectExplorer::idFromMap(map));

    BuildConfiguration * bc = static_cast<BuildConfiguration *>(parent->activeBuildConfiguration());
    if (!bc)
        return nullptr;

    const info::Recipe * recipe = bc->find_recipe(element.uri);

    if (recipe)
        return ProjectExplorer::IRunConfigurationFactory::createHelper<RunConfiguration>(parent, recipe->to_id(), CookBuildTarget(*recipe));
    else
        return ProjectExplorer::IRunConfigurationFactory::createHelper<RunConfiguration>(parent, element.to_id(), CookBuildTarget(element));
}

info::Element RunConfigurationFactory::unsafe_to_element_(const Core::Id & id)
{
    info::Element element;
    if (!element.from_id(id))
        Utils::writeAssertLocation("Id is not assigned to a recipe");

    return element;
}

std::pair<info::Element, bool> RunConfigurationFactory::safe_to_element_(const Core::Id & id)
{
    std::pair<info::Element, bool> result;
    result.second = result.first.from_id(id);
    return result;
}

const info::Recipe * RunConfigurationFactory::find_recipe_(ProjectExplorer::Target *parent, const Core::Id & id) const
{
    auto p = safe_to_element_(id);
    if(!p.second)
        return nullptr;

    BuildConfiguration * bc = static_cast<BuildConfiguration *>(parent->activeBuildConfiguration());
    if (!bc)
        return nullptr;

    return bc->find_recipe(p.first.uri);
}




} }
