#include "cook/project/CookBuildTarget.hpp"

namespace cook { namespace project {

namespace {

CookBuildTarget generate_default_target(TargetType type, const QString & display_name)
{
    CookBuildTarget tgt;
    tgt.display_name = display_name;
    tgt.type = type;
    return tgt;
}

}

CookBuildTarget::CookBuildTarget()
    : type(Target_Undefined)
{

}

CookBuildTarget::CookBuildTarget(const info::Recipe & recipe)
    : type(Target_URI),
      uri(recipe.uri),
      display_name(info::display_name(recipe))
{
}

CookBuildTarget::CookBuildTarget(const info::BuildRecipe & recipe)
    : CookBuildTarget(static_cast<const info::Recipe &>(recipe))
{
    executable = recipe.build_target;
}

CookBuildTarget CookBuildTarget::default_target()
{
    static CookBuildTarget tgt_default = generate_default_target(Target_Default, "default");
    return tgt_default;
}

CookBuildTarget CookBuildTarget::current_executable()
{
    static CookBuildTarget tgt_current = generate_default_target(Target_CurrentExecutable, "current");
    return tgt_current;
}

bool CookBuildTarget::operator==(const CookBuildTarget & rhs) const
{
    if (type != rhs.type)
        return false;

    return (type != Target_URI) || (uri == rhs.uri && display_name == rhs.display_name && executable == rhs.executable);
}

} }
