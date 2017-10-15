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
    auto convert_command = [](const QString & cmd, const QStringList & args)
    {
        return QString("\"%1\" %2").arg(cmd).arg(args.join(" "));
    };

    connect(recipes_mgr_, &info::StructureManager::started,
            [&](const QString & cmd, const QStringList & args)
    {
        emit started(convert_command(cmd, args), InfoRequestType::Recipes);
    }
    );

    connect(recipes_mgr_, &info::StructureManager::error_occured,                     [this](const QString & error) { qDebug() << error; emit error_occurred(error, InfoRequestType::Recipes); } );
    connect(recipes_mgr_, &info::StructureManager::finished,                          [this](bool ok) { emit finished(ok, InfoRequestType::Recipes); } );
    connect(recipes_mgr_, &info::StructureManager::process_output, [this](const QByteArray & array) { emit process_output(QString::fromLatin1(array));});
}

InfoManager::~InfoManager()
{
//    safe_destruct(*recipes_mgr_);
}

} }
