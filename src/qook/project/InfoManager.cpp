#include "qook/project/InfoManager.hpp"
#include "qook/project/info/RecipesManager.hpp"
#include "qook/project/info/DetailedRecipesManager.hpp"

namespace qook { namespace project {

namespace  {

template <typename T>
void safe_destruct(info::AsyncManager<T> & t)
{
    QFuture<void> f = t.future();
    if (!f.isFinished())
    {
        f.cancel();
        f.waitForFinished();
        t.deleteLater();
    }
}

}

InfoManager::InfoManager(BuildConfiguration * config)
    : recipes_mgr_(new info::RecipesManager(config)),
      detailed_recipes_mgr_(new info::DetailedRecipesManager(config))
{
    connect(recipes_mgr_, &info::RecipesManager::started,                           [this]() {emit started(InfoRequestType::Recipes); } );
    connect(detailed_recipes_mgr_, &info::DetailedRecipesManager::started,          [this]() {emit started(InfoRequestType::Detailed_Recipes); } );

    connect(recipes_mgr_, &info::RecipesManager::error_occured,                     [this](const QString & error) { emit error_occurred(error, InfoRequestType::Recipes); } );
    connect(detailed_recipes_mgr_, &info::DetailedRecipesManager::error_occured,    [this](const QString & error) { emit error_occurred(error, InfoRequestType::Detailed_Recipes); } );

    connect(recipes_mgr_, &info::RecipesManager::finished,                          [this](bool ok) { emit finished(ok, InfoRequestType::Recipes); } );
    connect(detailed_recipes_mgr_, &info::DetailedRecipesManager::finished,         [this](bool ok) { emit finished(ok, InfoRequestType::Detailed_Recipes); } );
}

InfoManager::~InfoManager()
{
    safe_destruct(*recipes_mgr_);
    safe_destruct(*detailed_recipes_mgr_);
}

} }
