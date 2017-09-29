#ifndef HEADER_qook_project_ProjectNodes_hpp_ALREADY_INCLUDED
#define HEADER_qook_project_ProjectNodes_hpp_ALREADY_INCLUDED

#include <qook/project/info/Types.hpp>
#include <projectexplorer/projectnodes.h>

namespace qook { namespace project {

class Project;

class CookNode : public ProjectExplorer::ProjectNode
{
public:
    explicit CookNode(Project * project);

    virtual bool showInSimpleTree() const override;
    virtual QString tooltip() const override;

private:
    Project * project_;
};

class RecipeNode : public ProjectExplorer::ProjectNode
{
public:
    explicit RecipeNode(const info::BuildRecipe & recipe);

    void compress();
};

class ChaiScriptNode : public ProjectExplorer::FileNode
{
public:
    explicit ChaiScriptNode(const Utils::FileName & fn);

    virtual bool showWhenEmpty() const { return true; }
};

} }

#endif
