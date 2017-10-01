#ifndef HEADER_cook_toolset_KitInformation_hpp_ALREADY_INCLUDED
#define HEADER_cook_toolset_KitInformation_hpp_ALREADY_INCLUDED

#include <projectexplorer/kitinformation.h>

namespace cook { namespace toolset {

class Tool;
class CookTool;
class NinjaTool;

class KitInformation : public ProjectExplorer::KitInformation
{
    Q_OBJECT

public:
    KitInformation(const Core::Id & type_id, const QString & name);

//    static Core::Id id();

    static const CookTool * cook_tool(const ProjectExplorer::Kit *k);
    static bool set_cook_tool(ProjectExplorer::Kit *k, const Core::Id & id);
    static const NinjaTool * ninja_tool(const ProjectExplorer::Kit *k);
    static bool set_ninja_tool(ProjectExplorer::Kit *k, const Core::Id & id);

    const Tool * get_tool(const ProjectExplorer::Kit * k) const;
    bool set_tool(const Core::Id & tool_id, ProjectExplorer::Kit * k) const;
    static const Tool * get_tool(const Core::Id & type_id, const ProjectExplorer::Kit *k);
    static bool set_tool(const Core::Id & type_id, const Core::Id & tool_id, ProjectExplorer::Kit * k);

    // KitInformation interface
    virtual QVariant defaultValue(const ProjectExplorer::Kit *k) const final;
    virtual QList<ProjectExplorer::Task> validate(const ProjectExplorer::Kit *k) const final;
    virtual void setup(ProjectExplorer::Kit *k) final;
    virtual void fix(ProjectExplorer::Kit *k) final;
    virtual ItemList toUserOutput(const ProjectExplorer::Kit *k) const final;
    virtual ProjectExplorer::KitConfigWidget *createConfigWidget(ProjectExplorer::Kit *k) const final;
    virtual void upgrade(ProjectExplorer::Kit * k) final;

    virtual void addToMacroExpander(ProjectExplorer::Kit *k, Utils::MacroExpander *expander) const final;
    virtual QSet<Core::Id> availableFeatures(const ProjectExplorer::Kit *k) const final;

private:
    Core::Id type_id_;
    const QString name_;
};

} }

#endif
