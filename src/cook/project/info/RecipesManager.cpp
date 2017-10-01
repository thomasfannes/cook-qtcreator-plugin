#include "cook/project/info/AsyncManager.hxx"
#include "cook/project/info/RecipesManager.hpp"

namespace cook { namespace project { namespace info {

RecipesManager::RecipesManager(const BuildConfiguration * config)
    : AsyncManager<RecipesParser>(config)
{
}

bool RecipesManager::start_async()
{    
    return async_run_([](const BuildConfiguration * config) { return config->all_recipes_options(); });
}

} } }
