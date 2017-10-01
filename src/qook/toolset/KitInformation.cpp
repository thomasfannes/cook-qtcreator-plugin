#include "qook/toolset/KitInformation.hpp"
#include "qook/toolset/KitConfigWidget.hpp"
#include "qook/toolset/Tool.hpp"
#include "qook/toolset/NinjaTool.hpp"
#include "qook/toolset/CookTool.hpp"
#include "qook/toolset/Manager.hpp"
#include <projectexplorer/kitmanager.h>
#include <utils/qtcassert.h>
#include <utils/macroexpander.h>

namespace qook { namespace toolset {

namespace  {

Core::Id type_to_kit_id(const Core::Id & type_id) { return type_id.withPrefix("CookProjectManager.CookKitInformation."); }

}

KitInformation::KitInformation(const Core::Id & type_id, const QString & name)
    : type_id_(type_id),
      name_(name)
{
    setObjectName(QLatin1String("CookKitInformation"));
    setId(type_to_kit_id(type_id_));
    setPriority(20001);

    auto fix_kits = [this]()
    {
        for(ProjectExplorer::Kit * k : ProjectExplorer::KitManager::kits())
            fix(k);
    };

    connect(Manager::instance(), &Manager::tool_removed, fix_kits);
    connect(Manager::instance(), &Manager::tool_added, fix_kits);
}

const Tool * KitInformation::get_tool(const ProjectExplorer::Kit * k) const
{
    if (!k)
        return nullptr;
    return get_tool(type_id_, k);
}

const Tool * KitInformation::get_tool(const Core::Id & type_id, const ProjectExplorer::Kit * k)
{
    if(!k)
        return nullptr;

    const QVariant tool_id = k->value(type_to_kit_id(type_id));
    return Manager::instance()->find_registered_tool(Core::Id::fromSetting(tool_id));
}

bool KitInformation::set_tool(const Core::Id & tool_id, ProjectExplorer::Kit * k) const
{
    return set_tool(type_id_, tool_id, k);
}

bool KitInformation::set_tool(const Core::Id &type_id, const Core::Id & tool_id, ProjectExplorer::Kit * k)
{
    const Core::Id & to_set = tool_id.isValid() ? tool_id : Manager::instance()->default_tool_id(type_id);

    if(!to_set.isValid())
        return false;

    const Tool * tool = Manager::instance()->find_registered_tool(to_set);
    if(tool == nullptr || tool->type_id() != type_id)
        return false;

    if(!k)
        return false;

    k->setValue(type_to_kit_id(type_id), to_set.toSetting());
    return true;
}


const CookTool * KitInformation::cook_tool(const ProjectExplorer::Kit *k)
{
    return dynamic_cast<const CookTool *>(get_tool(CookTool::type_id(), k));
}

const NinjaTool *KitInformation::ninja_tool(const ProjectExplorer::Kit *k)
{
    return dynamic_cast<const NinjaTool *>(get_tool(NinjaTool::type_id(), k));
}

bool KitInformation::set_cook_tool(ProjectExplorer::Kit *k, const Core::Id & id)
{
    return set_tool(CookTool::type_id(), id, k);
}

bool KitInformation::set_ninja_tool(ProjectExplorer::Kit *k, const Core::Id & id)
{
    return set_tool(NinjaTool::type_id(), id, k);
}

void KitInformation::upgrade(ProjectExplorer::Kit * k)
{
    // check if the tool exists
    fix(k);
}

QVariant KitInformation::defaultValue(const ProjectExplorer::Kit *k) const
{
    const Core::Id id = k ? Manager::instance()->default_tool_id(type_id_) : Core::Id();
    return id.toSetting();
}

QList<ProjectExplorer::Task> KitInformation::validate(const ProjectExplorer::Kit * /*k*/) const
{
    return {};
}

void KitInformation::setup(ProjectExplorer::Kit *k)
{
    const Tool * tool = get_tool(k);

    if (!tool)
        set_tool(Manager::instance()->default_tool_id(type_id_), k);
}

void KitInformation::fix(ProjectExplorer::Kit *k)
{
    if (!KitInformation::cook_tool(k) || !KitInformation::ninja_tool(k))
        setup(k);
}

KitInformation::ItemList KitInformation::toUserOutput(const ProjectExplorer::Kit *k) const
{
    const Tool * tool = get_tool(k);
    return ItemList() << qMakePair(name_, tool == 0 ? tr("Unconfigured") : tool->display_name());
}

ProjectExplorer::KitConfigWidget * KitInformation::createConfigWidget(ProjectExplorer::Kit *k) const
{
    return new KitConfigWidget(k, this, type_id_, name_);
}

void KitInformation::addToMacroExpander(ProjectExplorer::Kit *k, Utils::MacroExpander *expander) const
{
    QString name = QString("test:Executable").arg(name_);
    expander->registerFileVariables(name.toLatin1(), QString("Path to the %1 executable").arg(name_), [this, k]()
    {
        const Tool * tool = get_tool(k);
        return tool ? tool->user_file_name() : QString();
    });
}

QSet<Core::Id> KitInformation::availableFeatures(const ProjectExplorer::Kit */*k*/) const
{
    return {};
}


} } // namespace qook
