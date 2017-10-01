#ifndef HEADER_cook_project_info_Types_hpp_ALREADY_INCLUDED
#define HEADER_cook_project_info_Types_hpp_ALREADY_INCLUDED

#include <utils/fileutils.h>
#include <coreplugin/id.h>
#include <QString>
#include <QVector>
#include <QMap>

namespace cook { namespace project { namespace info {

enum class TargetType
{
    Executable,
    StaticLibrary,
    Unknown
};

enum class FileType
{
    Unknown = 0,
    Source,
    Header,

};

struct FileInfo
{
    FileType type;
    Utils::FileName file;
};

struct Recipe
{
    QString uri;
    QString name;

    Core::Id to_id() const;
    bool from_id(const Core::Id & id);
};

struct BuildRecipe : public Recipe
{
    Utils::FileName script;
    TargetType type;
    Utils::FileName build_target;

    QVector<FileInfo> files;
    QStringList include_paths;
};

struct Recipes
{
    QMap<QString, Recipe> recipes;
    QString default_uri;
};

struct BuildRecipes
{
    QMap<QString, BuildRecipe> recipes;
    QString default_uri;
};

inline QString display_name(const Recipe & recipe)
{
    QString val = recipe.uri;

    if (recipe.name.isEmpty())
        val.append(QString(" (%1)").arg(recipe.name));

    return val;
}



} } }

#endif
