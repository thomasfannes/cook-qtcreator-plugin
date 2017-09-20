#ifndef HEADER_qook_toolset_KitInformation_hpp_ALREADY_INCLUDED
#define HEADER_qook_toolset_KitInformation_hpp_ALREADY_INCLUDED

#include <projectexplorer/kitinformation.h>

namespace qook { namespace toolset {

class Tool;

class KitInformation : public ProjectExplorer::KitInformation
{
    Q_OBJECT
public:
    KitInformation();

    static Core::Id id();

    static const Tool * tool(const ProjectExplorer::Kit *k);
    static void set_tool(ProjectExplorer::Kit *k, const Core::Id & id);

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
};

} }

#endif
