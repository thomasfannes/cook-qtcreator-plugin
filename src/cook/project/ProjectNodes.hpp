#ifndef HEADER_cook_project_ProjectNodes_hpp_ALREADY_INCLUDED
#define HEADER_cook_project_ProjectNodes_hpp_ALREADY_INCLUDED

#include "cook/project/info/Types.hpp"
#include <projectexplorer/projectnodes.h>

namespace cook { namespace project {

class Project;



class CookNode : public ProjectExplorer::ProjectNode
{
public:
    enum Type
    {
        Type_RecipeNode,
        Type_BookNode,
        Type_RootNode
    };

    explicit CookNode(const info::Recipe & recipe, const Type & type);

    virtual bool showInSimpleTree() const override;

    void compress();
};

class RecipesNode : public ProjectExplorer::ProjectNode
{
public:
    explicit RecipesNode(const Utils::FileName & dir);
};

class ChaiScriptNode : public ProjectExplorer::FileNode
{
public:
    explicit ChaiScriptNode(const Utils::FileName & fn);

    virtual bool showWhenEmpty() const { return true; }
};

} }

#endif
