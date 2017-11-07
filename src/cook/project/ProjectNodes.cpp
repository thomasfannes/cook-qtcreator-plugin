#include "cook/project/ProjectNodes.hpp"
#include "cook/project/Project.hpp"

namespace cook { namespace project {

CookNode::CookNode(const info::Recipe & recipe, const Type &type)
    : ProjectExplorer::ProjectNode(recipe.script.parentDir())
{
    setDisplayName(info::short_display_name(recipe));

    switch(type)
    {
        case Type::Type_BookNode:
            setIcon((QIcon(":/icon/book")));
            break;

        case Type::Type_RecipeNode:
            setIcon((QIcon(":/icon/recipe")));
            break;

        case Type::Type_RootNode:
            setIcon((QIcon(":/icon/cook")));
            break;
    }
}

bool CookNode::showInSimpleTree() const
{
    return false;
}

void CookNode::compress()
{
    for(auto * n : nodes())
        if (auto * f = n->asFolderNode())
            f->compress();
}

RecipesNode::RecipesNode(const Utils::FileName & dir)
    : ProjectExplorer::ProjectNode(dir)
{
    setIcon(QIcon(":/icon/book"));
    setPriority(-100);
}



ChaiScriptNode::ChaiScriptNode(const Utils::FileName & fn)
    : ProjectExplorer::FileNode(fn, ProjectExplorer::FileType::Unknown, false)
{
//    setIcon(QIcon(":/icon/chai"));
}

} }
