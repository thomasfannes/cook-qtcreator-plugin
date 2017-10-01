#ifndef HEADER_cook_project_info_RecipesManager_hpp_ALREADY_INCLUDED
#define HEADER_cook_project_info_RecipesManager_hpp_ALREADY_INCLUDED

#include "cook/project/info/RecipesParser.hpp"
#include "cook/project/info/AsyncManager.hpp"

namespace cook { namespace project { namespace info {

class RecipesManager : public AsyncManager<RecipesParser>
{
public:
    RecipesManager(const BuildConfiguration *config);

    bool start_async();
};

} } }

#endif
