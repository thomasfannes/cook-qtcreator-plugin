#include "qook/project/BuildNinjaManager.hpp"
#include "qook/project/BuildConfiguration.hpp"
#include "qook/toolset/CookTool.hpp"
#include <QtConcurrent>
#include <coreplugin/messagemanager.h>

namespace qook { namespace project {

namespace  {

QString construct_ninja_file(const QString & uri, const BuildConfiguration * config)
{
    if (!config->is_valid_uri(uri))
        return QString("Invalid uri: <%1>").arg(uri);

    const toolset::CookTool * tool = config->tool();
    if (!tool && !tool->is_valid())
        return QString("No valid cook tool specified. Please update the kit");

    QStringList options = config->build_options(uri);
    QString cmd = QString("[cook]: %1 %2").arg(tool->user_file_name()).arg(options.join(" "));
    Core::MessageManager::write(cmd, Core::MessageManager::Silent);
    Utils::SynchronousProcessResponse response = tool->run(options);

    if (!response.exitCode == response.Finished)
        return QString("Error running cook: %1").arg(response.allOutput());

    return QString();
}
}

BuildNinjaManager::BuildNinjaManager(BuildConfiguration *config)
    : config_(config)
{
    connect(&watcher_, &Watcher::finished, this, &BuildNinjaManager::run_finished_);
    connect(&watcher_, &Watcher::started, this, &BuildNinjaManager::started);
}

QFuture<void> BuildNinjaManager::future() const
{
    return future_;
}

void BuildNinjaManager::run_finished_()
{
    if (!future_.isFinished())
        return;

    QString error = future_.result();
    if (!error.isNull())
        emit error_occured(error);

    emit finished(error.isNull());
}

bool BuildNinjaManager::start_async()
{
    if (!future_.isFinished())
        return false;

    future_ = QtConcurrent::run(&construct_ninja_file, config_->target_uri(), config_);
    watcher_.setFuture(future_);

    return true;
}

} }
