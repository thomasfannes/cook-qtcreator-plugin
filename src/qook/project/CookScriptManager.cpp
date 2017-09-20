#include "qook/project/CookScriptManager.hpp"
#include "qook/project/BuildConfiguration.hpp"

#include <utils/qtcassert.h>
#include <QtConcurrent>

namespace qook { namespace project {

namespace {

ScanResult perform_run(const BuildConfiguration * config)
{
    const toolset::Tool * tool = config->tool();
    if(tool == nullptr)
    {
        ScanResult result;
        result.error = "No valid tool set";
        result.result_code = ScanResult::ToolFailed;

        return result;
    }

    QStringList options = config->tree_format_options();

    RecipeScanRun run(tool, options);
    run();

    return run.result();
}

}

CookScriptManager::CookScriptManager(BuildConfiguration * configuration)
    : config_(configuration)
{
    connect(&watcher_, &FutureWatcher::finished, this, &CookScriptManager::finished);

}

bool CookScriptManager::start_async_parse()
{
    if(!future_.isFinished())
        return false;

    emit started();

    // start a new
    future_ = QtConcurrent::run(&perform_run, config_);
    watcher_.setFuture(future_);

    return true;
}

QFuture<void> CookScriptManager::future()
{
    return future_;
}

void CookScriptManager::finished()
{
    ScanResult res = future_.result();

    if (res.result_code != ScanResult::Ok)
    {
        emit error_occured(res.error);
    }
    else
    {
        last_ = res.cook;
        emit data_available();
    }
}

} }
