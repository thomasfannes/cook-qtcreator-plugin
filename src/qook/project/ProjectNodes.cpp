#include "qook/project/ProjectNodes.hpp"
#include "qook/project/Project.hpp"
#include "qook/project/Structure.hpp"

namespace qook { namespace project {

CookNode::CookNode(Project * project)
    : ProjectExplorer::ProjectNode(project->projectDirectory()),
      project_(project)
{
    setIcon(QIcon(":/icon/cook"));
}

bool CookNode::showInSimpleTree() const
{
    return false;
}

QString CookNode::tooltip() const
{
    return project_->displayName();
}

RecipeNode::RecipeNode(const Recipe &recipe)
    : ProjectExplorer::ProjectNode(recipe.script.parentDir())
{
    QString name = recipe.uri;
    if(!recipe.display_name.isEmpty())
        name.append(QString(" (%1)").arg(recipe.display_name));

    setDisplayName( name );
    setIcon(QIcon(":/icon/recipe"));
}

void RecipeNode::compress()
{
    for(auto * n : nodes())
        if (auto * f = n->asFolderNode())
            f->compress();
}

ChaiScriptNode::ChaiScriptNode(const Utils::FileName & fn)
    : ProjectExplorer::FileNode(fn, ProjectExplorer::FileType::Unknown, false)
{
//    setIcon(QIcon(":/icon/chai"));
}

} }
