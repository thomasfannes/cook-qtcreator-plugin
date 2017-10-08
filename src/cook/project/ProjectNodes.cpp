#include "cook/project/ProjectNodes.hpp"
#include "cook/project/Project.hpp"

namespace cook { namespace project {

CookNode::CookNode(Project * project)
    : ProjectExplorer::ProjectNode(project->projectDirectory()),
      project_(project)
{
    setIcon(QIcon(":/icon/cook"));
}

CookNode::CookNode(Project * project, const Utils::FileName & dir)
    : ProjectExplorer::ProjectNode(dir),
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

RecipeNode::RecipeNode(const info::Recipe &recipe)
    : ProjectExplorer::ProjectNode(recipe.script.parentDir())
{
    QString name = recipe.tag;
    if(!recipe.name.isEmpty())
        name.append(QString(" (%1)").arg(recipe.name));

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
