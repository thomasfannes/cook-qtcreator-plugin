#ifndef HEADER_qook_project_CookBuildTarget_hpp_ALREADY_INCLUDED
#define HEADER_qook_project_CookBuildTarget_hpp_ALREADY_INCLUDED

#include "qook/project/info/Types.hpp"
#include <utils/fileutils.h>
#include <QString>
#include <QMetaType>

namespace qook { namespace project {

struct CookBuildTarget
{
    CookBuildTarget()
        : is_special(false)
    {
    }

    explicit CookBuildTarget(const info::Recipe & recipe)
        : display_name(info::display_name(recipe)),
          uri(recipe.uri),
          executable(Utils::FileName()),
          is_special(false)
    {
    }

    explicit CookBuildTarget(const info::BuildRecipe & recipe)
        : CookBuildTarget(static_cast<const info::Recipe &>(recipe))
    {
        executable = recipe.build_target;
    }

    explicit CookBuildTarget(const QString & special_name)
        : display_name(special_name),
          is_special(true)
    {
    }

    static CookBuildTarget default_target()         { return CookBuildTarget ("default"); }
    static CookBuildTarget current_executable()     { return CookBuildTarget ("current executable"); }


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
