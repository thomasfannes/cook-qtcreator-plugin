#include "cook/project/info/BuildNinjaManager.hpp"
#include "cook/project/BuildConfiguration.hpp"
#include "cook/toolset/CookTool.hpp"

#include <coreplugin/messagemanager.h>

#include <QtConcurrent>
#include <QFuture>
#include <QFutureWatcher>
#include <QObject>

namespace cook { namespace project { namespace info {

namespace  {

QString run(const BuildConfiguration * config)
{
    QString result;

    // try the tool
    const toolset::CookTool * tool = config->tool();
    if(tool == nullptr || !tool->is_valid())
    {
        result = "The specified Cook tool is not valid. Please update the kit";
        return result;
    }

    // try to create the directory
    {
        const QString & dir_str = config->buildDirectory().toString();
        if (!QDir(dir_str).mkpath("."))
        {
            result = QString("Cannot create build directory '%1'").arg(dir_str);
            return result;
        }
    }

    QStringList options = config->build_options(config->target_uri());
    QString cmd = QString("[cook]: %1 %2").arg(tool->user_file_name()).arg(options.join(" "));
    Core::MessageManager::write(cmd, Core::MessageManager::Silent);

    // execute the tool
    Utils::SynchronousProcessResponse respons = tool->run(options);
    Core::MessageManager::write(respons.allOutput(), Core::MessageManager::Silent);

    if(respons.exitCode != Utils::SynchronousProcessResponse::Finished)
    {
        result = QString("The Cook tool failed:\n%1").arg(respons.allOutput());
        Core::MessageManager::write(result, Core::MessageManager::Silent);
        return result;
    }

    return result;
}

}

BuildNinjaManager::BuildNinjaManager(const BuildConfiguration *config)
    : config_(config)
{
    connect(&watcher_, &Watcher::finished, this, &BuildNinjaManager::run_finished);
    connect(&watcher_, &Watcher::started, this, &BuildNinjaManager::started);
}

bool BuildNinjaManager::start_async()
{
    if (!future_.isFinished())
        return false;

    future_ = QtConcurrent::run(run, config_);
    watcher_.setFuture(future_);

    return true;
}

void BuildNinjaManager::run_finished()
{
    if (!future_.isFinished())
        return;

    QString error = future_.result();
    if(!error.isNull())
        emit error_occured(error);

    emit finished(error.isNull());
}


} } }

