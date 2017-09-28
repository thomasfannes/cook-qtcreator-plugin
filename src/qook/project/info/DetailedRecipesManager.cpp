#include "qook/project/info/AsyncManager.hxx"
#include "qook/project/info/DetailedRecipesManager.hpp"

namespace qook { namespace project { namespace info {

DetailedRecipesManager::DetailedRecipesManager(const BuildConfiguration * config)
    : AsyncManager<DetailedRecipesParser>(config)
{

}

bool DetailedRecipesManager::start_async(const QString & uri)
{
    return async_run_([=](const BuildConfiguration * config) { return config->recipe_detail_options(uri); });
}

} } }
