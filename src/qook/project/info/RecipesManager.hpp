#ifndef HEADER_qook_project_info_RecipesManager_hpp_ALREADY_INCLUDED
#define HEADER_qook_project_info_RecipesManager_hpp_ALREADY_INCLUDED

#include "qook/project/info/RecipesParser.hpp"
#include "qook/project/info/AsyncManager.hpp"

namespace qook { namespace project { namespace info {

class RecipesManager : public AsyncManager<RecipesParser>
{
public:
    RecipesManager(const BuildConfiguration *config);

    bool start_async();
};

} } }

#endif
