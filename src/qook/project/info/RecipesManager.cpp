#include "qook/project/info/AsyncManager.hxx"
#include "qook/project/info/RecipesManager.hpp"

namespace qook { namespace project { namespace info {

RecipesManager::RecipesManager(const BuildConfiguration * config)
    : AsyncManager<RecipesParser>(config)
{
}

bool RecipesManager::start_async()
{    
    return async_run_([](const BuildConfiguration * config) { return config->all_recipes_options(); });
}

} } }
