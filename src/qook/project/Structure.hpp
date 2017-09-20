#ifndef HEADER_qook_project_Structure_hpp_ALREADY_INCLUDED
#define HEADER_qook_project_Structure_hpp_ALREADY_INCLUDED

#include <QString>
#include <QFileInfo>
#include <QVector>
#include <utils/fileutils.h>

namespace qook { namespace project {

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
    QString tag;
    Utils::FileName script;
    QString display_name;
    TargetType type;
    Utils::FileName build_target;

    QVector<FileInfo> files;
    QStringList include_paths;
};

struct Book
{
    QString uri;
    Utils::FileName script;
    QString display_name;

    QVector<QString> subboooks;
    QVector<QString> recipes;
};

struct Cook
{
    QVector<Book> books;
    QVector<Recipe> recipes;

    QString root_uri;
};


} }



#endif
