#ifndef HEADER_qook_project_CookBuildTarget_hpp_ALREADY_INCLUDED
#define HEADER_qook_project_CookBuildTarget_hpp_ALREADY_INCLUDED

#include "qook/project/info/Types.hpp"
#include <utils/fileutils.h>
#include <QString>
#include <QMetaType>

namespace qook { namespace project {

enum TargetType
{
    Target_Undefined,
    Target_URI,
    Target_CurrentExecutable,
    Target_Default,
};

struct CookBuildTarget
{
    CookBuildTarget();
    explicit CookBuildTarget(const info::Recipe & recipe);
    explicit CookBuildTarget(const info::BuildRecipe & recipe);

    static CookBuildTarget default_target();
    static CookBuildTarget current_executable();

    bool operator==(const CookBuildTarget & rhs) const;

    TargetType type;
    QString uri;
    QString display_name;
    Utils::FileName executable;
};

} }

Q_DECLARE_METATYPE(qook::project::CookBuildTarget)

#endif
