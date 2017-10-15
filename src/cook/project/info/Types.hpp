#ifndef HEADER_cook_project_info_Types_hpp_ALREADY_INCLUDED
#define HEADER_cook_project_info_Types_hpp_ALREADY_INCLUDED

#include <utils/fileutils.h>
#include <coreplugin/id.h>
#include <QString>
#include <QVector>
#include <QMap>
#include <QSet>
#include <QSharedPointer>

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

struct Element
{
    QString uri;
    QString name;
    Utils::FileName script;

    Core::Id to_id() const;
    bool from_id(const Core::Id & id);
};

struct Recipe : public Element
{
    explicit Recipe(bool is_recipe_flag = false)
        : is_recipe(is_recipe_flag)
    {
    }

    QVector<Recipe> children;

    bool is_recipe;
    QString tag;
    TargetType type;
    Utils::FileName build_target;

    QSet<Utils::FileName> all_scripts;
    QVector<FileInfo> files;
    QStringList include_paths;

    void clear()
    {
        children.clear();
        is_recipe = false;
        tag.clear();
        build_target.clear();
        files.clear();
        include_paths.clear();
    }
};

struct Recipes
{
    Recipe root;
    QString default_uri;

    void clear()
    {
        root.clear();
        default_uri.clear();
    }
};

inline QString display_name(const Element & recipe)
{
    QString val = recipe.uri;

    if (!recipe.name.isEmpty())
        val.append(QString(" (%1)").arg(recipe.name));

    return val;
}



} } }

#endif
