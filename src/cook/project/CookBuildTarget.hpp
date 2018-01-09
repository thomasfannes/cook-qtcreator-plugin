#ifndef HEADER_cook_project_CookBuildTarget_hpp_ALREADY_INCLUDED
#define HEADER_cook_project_CookBuildTarget_hpp_ALREADY_INCLUDED

#include "cook/project/info/Types.hpp"
#include <utils/fileutils.h>
#include <QString>
#include <QMetaType>

namespace cook { namespace project {

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
    explicit CookBuildTarget(const info::Element & element);
    explicit CookBuildTarget(const info::Recipe & recipe);

    static CookBuildTarget current_executable();

    bool operator==(const CookBuildTarget & rhs) const;

    TargetType type;
    QString uri;
    QString display_name;
    Utils::FileName executable;
};

} }

Q_DECLARE_METATYPE(cook::project::CookBuildTarget)

#endif
