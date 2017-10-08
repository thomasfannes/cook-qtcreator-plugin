#ifndef HEADER_cook_project_InfoManagerType_hpp_ALREADY_INCLUDED
#define HEADER_cook_project_InfoManagerType_hpp_ALREADY_INCLUDED

#include <QFlags>

namespace cook { namespace project {

enum InfoRequestType
{
    Unknown = 0x00,
    Recipes = 0x01,
    Ninja = 0x02
};

Q_DECLARE_FLAGS(RequestFlags, InfoRequestType)


} }

Q_DECLARE_OPERATORS_FOR_FLAGS(cook::project::RequestFlags)

#endif
