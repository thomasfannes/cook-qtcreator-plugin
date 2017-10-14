#include "cook/project/InfoManager.hpp"

namespace cook { namespace project {

//namespace  {

//template <typename T>
//void safe_destruct(info::AsyncManager<T> & t)
//{
//    QFuture<void> f = t.future();
//    if (!f.isFinished())
//    {
//        f.cancel();
//        f.waitForFinished();
//        t.deleteLater();
//    }
//}

//}

InfoManager::InfoManager(BuildConfiguration * config)
    : recipes_mgr_(new info::StructureManager(config))
{
    connect(recipes_mgr_, &info::StructureManager::started,                           [this]() {emit started(InfoRequestType::Recipes); } );

    connect(recipes_mgr_, &info::StructureManager::error_occured,                     [this](const QString & error) { emit error_occurred(error, InfoRequestType::Recipes); } );

    connect(recipes_mgr_, &info::StructureManager::finished,                          [this](bool ok) { emit finished(ok, InfoRequestType::Recipes); } );
}

InfoManager::~InfoManager()
{
//    safe_destruct(*recipes_mgr_);
}

} }
