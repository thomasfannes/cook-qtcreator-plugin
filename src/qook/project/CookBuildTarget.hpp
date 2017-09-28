#ifndef HEADER_qook_project_CookBuildTarget_hpp_ALREADY_INCLUDED
#define HEADER_qook_project_CookBuildTarget_hpp_ALREADY_INCLUDED

#include <QString>
#include <QMetaType>
#include <utils/fileutils.h>

namespace qook { namespace project {

struct CookBuildTarget
{
    CookBuildTarget()
        : is_special(false)
    {
    }

    CookBuildTarget(const QString & name, const QString & uri, const Utils::FileName & working_directory, bool is_special = false)
        : display_name(name),
          uri(uri),
          executable(working_directory),
          is_special(is_special)
    {
        executable.appendPath(uri);
    }

    static CookBuildTarget default_target()         { return CookBuildTarget ("default", "", Utils::FileName(), true); }
    static CookBuildTarget current_executable()     { return CookBuildTarget ("current executable", "", Utils::FileName(), true); }

    bool operator==(const CookBuildTarget & rhs) const
    {
        return display_name == rhs.display_name
                && uri == rhs.uri
                && executable == rhs.executable
                && is_special == rhs.is_special;
    }

    QString display_name;
    QString uri;
    Utils::FileName executable;
    bool is_special;
};


} }

Q_DECLARE_METATYPE(qook::project::CookBuildTarget)

#endif
