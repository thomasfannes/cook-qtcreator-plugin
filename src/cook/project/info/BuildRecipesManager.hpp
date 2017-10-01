#ifndef HEADER_cook_project_info_BuildRecipesManager_hpp_ALREADY_INCLUDED
#define HEADER_cook_project_info_BuildRecipesManager_hpp_ALREADY_INCLUDED

#include "cook/project/info/AsyncManager.hpp"
#include "cook/project/info/BuildRecipesParser.hpp"

namespace cook { namespace project { namespace info {

class BuildRecipesManager : public AsyncManager<BuildRecipesParser>
{
public:
    BuildRecipesManager(const BuildConfiguration *config);

    bool start_async();
};

} } }

#endif
