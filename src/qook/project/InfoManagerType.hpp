#ifndef HEADER_qook_project_InfoManagerType_hpp_ALREADY_INCLUDED
#define HEADER_qook_project_InfoManagerType_hpp_ALREADY_INCLUDED

#include <QFlags>

namespace qook { namespace project {

enum InfoRequestType
{
    Unknown = 0x00,
    Recipes = 0x01,
    Detailed_Recipes = 0x02,
};

Q_DECLARE_FLAGS(RequestFlags, InfoRequestType)


} }

Q_DECLARE_OPERATORS_FOR_FLAGS(qook::project::RequestFlags)

#endif
