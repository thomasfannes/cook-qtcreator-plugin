#include "qook/project/RunConfigurationFactory.hpp"
#include "qook/project/RunConfiguration.hpp"
#include "qook/project/BuildConfiguration.hpp"
#include "qook/project/Project.hpp"
#include <projectexplorer/target.h>

namespace qook { namespace project {


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

    return safe_to_recipe_(ProjectExplorer::idFromMap(map)).second;
}

ProjectExplorer::RunConfiguration * RunConfigurationFactory::clone(ProjectExplorer::Target *parent, ProjectExplorer::RunConfiguration * source)
{
    if (!canClone(parent, source))
        return 0;

    RunConfiguration * rc = static_cast<RunConfiguration*>(source);
    return new RunConfiguration(parent, rc);
}

QString RunConfigurationFactory::displayNameForId(Core::Id id) const
{
    return info::display_name(unsafe_to_recipe_(id));
}

QList<Core::Id> RunConfigurationFactory::availableCreationIds(ProjectExplorer::Target *parent, CreationMode mode) const
{
    Q_UNUSED(mode)
    if (!can_handle(parent))
        return QList<Core::Id>();

    BuildConfiguration * bc = static_cast<BuildConfiguration *>(parent->activeBuildConfiguration());
    QList<Core::Id> allIds;

    for(const info::BuildRecipe & recipe : bc->build_recipes_info().recipes)
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
    const info::BuildRecipe * build_recipe = find_recipe_(parent, id);
    if(!build_recipe)
        return nullptr;

    return new RunConfiguration(parent, build_recipe->to_id(), CookBuildTarget(*build_recipe));
}

ProjectExplorer::RunConfiguration * RunConfigurationFactory::doRestore(ProjectExplorer::Target *parent, const QVariantMap &map)
{
    info::Recipe recipe = unsafe_to_recipe_(ProjectExplorer::idFromMap(map));

    BuildConfiguration * bc = static_cast<BuildConfiguration *>(parent->activeBuildConfiguration());
    if (!bc)
        return nullptr;

    const info::BuildRecipe * build_recipe = bc->find_build_recipe(recipe.uri);
    if (build_recipe)
        return new RunConfiguration(parent, build_recipe->to_id(), CookBuildTarget(*build_recipe));
    else
        return new RunConfiguration(parent, recipe.to_id(), CookBuildTarget(recipe));
}

info::Recipe RunConfigurationFactory::unsafe_to_recipe_(const Core::Id & id)
{
    info::Recipe recipe;
    if (!recipe.from_id(id))
        Utils::writeAssertLocation("Id is not assigned to a recipe");

    return recipe;
}

std::pair<info::Recipe, bool> RunConfigurationFactory::safe_to_recipe_(const Core::Id & id)
{
    std::pair<info::Recipe, bool> result;
    result.second = result.first.from_id(id);
    return result;
}

const info::BuildRecipe * RunConfigurationFactory::find_recipe_(ProjectExplorer::Target *parent, const Core::Id & id) const
{
    auto p = safe_to_recipe_(id);
    if(!p.second)
        return nullptr;

    BuildConfiguration * bc = static_cast<BuildConfiguration *>(parent->activeBuildConfiguration());
    if (!bc)
        return nullptr;

    return bc->find_build_recipe(p.first.uri);
}




} }
