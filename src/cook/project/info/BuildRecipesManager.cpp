#include "cook/project/info/AsyncManager.hxx"
#include "cook/project/info/BuildRecipesManager.hpp"

namespace cook { namespace project { namespace info {

BuildRecipesManager::BuildRecipesManager(const BuildConfiguration * config)
    : AsyncManager<BuildRecipesParser>(config)
{

}

bool BuildRecipesManager::start_async()
{
    return async_run_([=](const BuildConfiguration * config) { return config->recipe_detail_options(config->target_uri()); });
}

} } }
