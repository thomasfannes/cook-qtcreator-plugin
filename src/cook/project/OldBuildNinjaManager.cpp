#include "cook/project/OldBuildNinjaManager.hpp"
#include "cook/project/BuildConfiguration.hpp"
#include "cook/toolset/CookTool.hpp"
#include <QtConcurrent>
#include <coreplugin/messagemanager.h>

namespace cook { namespace project {

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

    Core::MessageManager::write(response.allOutput(), Core::MessageManager::Silent);

    if (!response.exitCode == response.Finished)
    {
        QString error = QString("Error running cook: %1").arg(response.allOutput());
        Core::MessageManager::write(error, Core::MessageManager::Silent);

        return error;
    }

    return QString();
}
}

OldBuildNinjaManager::OldBuildNinjaManager(BuildConfiguration *config)
    : config_(config)
{
    connect(&watcher_, &Watcher::finished, this, &OldBuildNinjaManager::run_finished_);
    connect(&watcher_, &Watcher::started, this, &OldBuildNinjaManager::started);
}

QFuture<void> OldBuildNinjaManager::future() const
{
    return future_;
}

void OldBuildNinjaManager::run_finished_()
{
    if (!future_.isFinished())
        return;

    QString error = future_.result();
    if (!error.isNull())
        emit error_occured(error);

    emit finished(error.isNull());
}

bool OldBuildNinjaManager::start_async()
{
    if (!future_.isFinished())
        return false;

    future_ = QtConcurrent::run(&construct_ninja_file, config_->target_uri(), config_);
    watcher_.setFuture(future_);

    return true;
}

} }
