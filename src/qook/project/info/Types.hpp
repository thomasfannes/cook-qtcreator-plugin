#ifndef HEADER_qook_project_info_Types_hpp_ALREADY_INCLUDED
#define HEADER_qook_project_info_Types_hpp_ALREADY_INCLUDED

#include <utils/fileutils.h>
#include <QString>
#include <QVector>

namespace qook { namespace project { namespace info {

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
    QString display_name;
};


struct DetailedRecipe
{
    QString uri;
    QString display_name;
    Utils::FileName script;
    TargetType type;
    Utils::FileName build_target;

    QVector<FileInfo> files;
    QStringList include_paths;
};

struct Recipes
{
    QVector<Recipe> recipes;
    QString default_uri;
};

struct DetailedRecipes
{
    QVector<DetailedRecipe> recipes;
    QString default_uri;
};


} } }

#endif
