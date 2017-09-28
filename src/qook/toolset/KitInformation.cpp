#include "qook/toolset/KitInformation.hpp"
#include "qook/toolset/KitConfigWidget.hpp"
#include "qook/toolset/Tool.hpp"
#include "qook/toolset/Manager.hpp"
#include <projectexplorer/kitmanager.h>
#include <utils/qtcassert.h>
#include <utils/macroexpander.h>

namespace qook { namespace toolset {

namespace  {

Core::Id default_cook_tool_id()
{
    return Manager::instance()->default_tool_id();
}

const char TOOL_ID[] = "CookProjectManager.CookKitInformation";

}

KitInformation::KitInformation()
{
    setObjectName(QLatin1String("CookKitInformation"));
    setId(TOOL_ID);
    setPriority(20001);

    auto fix_kits = [this]()
    {
        for(ProjectExplorer::Kit * k : ProjectExplorer::KitManager::kits())
            fix(k);
    };

    connect(Manager::instance(), &Manager::tool_removed, fix_kits);
    connect(Manager::instance(), &Manager::tool_added, fix_kits);
}

Core::Id KitInformation::id()
{
    return TOOL_ID;
}

const Tool * KitInformation::tool(const ProjectExplorer::Kit *k)
{
    if(!k)
        return nullptr;

    Manager * mgr = Manager::instance();

    const QVariant id = k->value(TOOL_ID);
    return mgr->find_registered_tool(Core::Id::fromSetting(id));
}

void KitInformation::upgrade(ProjectExplorer::Kit * k)
{
    // check if the tool exists
    fix(k);
}

void KitInformation::set_tool(ProjectExplorer::Kit *k, const Core::Id & id)
{
    const Core::Id & to_set = id.isValid() ? id : default_cook_tool_id();
    QTC_ASSERT(!to_set.isValid() || Manager::instance()->find_registered_tool(to_set), return );

    if (k) k->setValue(TOOL_ID, to_set.toSetting());
}

QVariant KitInformation::defaultValue(const ProjectExplorer::Kit *k) const
{
    const Core::Id id = k ? default_cook_tool_id() : Core::Id();
    return id.toSetting();
}

QList<ProjectExplorer::Task> KitInformation::validate(const ProjectExplorer::Kit *k) const
{
    return {};
}

void KitInformation::setup(ProjectExplorer::Kit *k)
{
    const Tool * tool = KitInformation::tool(k);
    if (!tool)
        set_tool(k, default_cook_tool_id());
}

void KitInformation::fix(ProjectExplorer::Kit *k)
{
    if (!KitInformation::tool(k))
        setup(k);
}

KitInformation::ItemList KitInformation::toUserOutput(const ProjectExplorer::Kit *k) const
{
    const Tool * const t = tool(k);
    return ItemList() << qMakePair(tr("Cook"), t == 0 ? tr("Unconfigured") : t->display_name());
}

ProjectExplorer::KitConfigWidget * KitInformation::createConfigWidget(ProjectExplorer::Kit *k) const
{
    return new KitConfigWidget(k, this);
}

void KitInformation::addToMacroExpander(ProjectExplorer::Kit *k, Utils::MacroExpander *expander) const
{
    expander->registerFileVariables("Cook:Executable", tr("Path to the cook executable"),
                                    [this, k]()
    {
        const Tool * tool = KitInformation::tool(k);
        return tool ? tool->user_file_name() : QString();
    });
}

QSet<Core::Id> KitInformation::availableFeatures(const ProjectExplorer::Kit *k) const
{
    return {};
}


} } // namespace qook
