#ifndef HEADER_cook_project_info_AsyncProcessManager_hxx_ALREADY_INCLUDED
#define HEADER_cook_project_info_AsyncProcessManager_hxx_ALREADY_INCLUDED

#include "cook/project/info/AsyncProcessManager.hpp"
#include "cook/project/BuildConfiguration.hpp"
#include <utils/qtcassert.h>
#include <QtConcurrent>

namespace cook { namespace project { namespace info {

template <typename Derived, typename Process>
AsyncProcessManager<Derived, Process>::AsyncProcessManager(const BuildConfiguration * config)
    : config_(config)
{

}

template <typename Derived, typename Process>
AsyncProcessManager<Derived, Process>::~AsyncProcessManager()
{
    if (!future_.isFinished())
    {
        future_.cancel();
        future_.waitForFinished();
    }
}

template <typename Derived, typename Process>
bool AsyncProcessManager<Derived, Process>::start_async()
{
    if (!future_.isFinished())
        return false;

    auto run = [&](const toolset::CookTool * tool, auto process_starter) -> Process *
    {
        Process * process = new Process(tool);

        // connect the signals
        connect(process, &Process::started,                this, &AsyncProcessManager::started);
        connect(process, &Process::finished,               this, &AsyncProcessManager::on_finished_);
        connect(process, &Process::error_occured,          this, &AsyncProcessManager::error_occured);
        connect(process, &Process::warning_occured,        this, &AsyncProcessManager::warning_occured);
        connect(process, &Process::standard_error_written, this, &AsyncProcessManager::process_output);
        connect(process, &Process::standard_out_written,   this, &AsyncProcessManager::process_output);

        if (!process_starter(*process))
        {
            process->deleteLater();
            return nullptr;
        }

        return process;
    };

    Derived & d = static_cast<Derived &>(*this);
    auto starter = d.generate_process_starter(*config_);
    future_ = QtConcurrent::run(run, config_->tool(), starter);

    return true;
}

template <typename Derived, typename Process>
void AsyncProcessManager<Derived, Process>::on_finished_(bool success)
{
    QTC_ASSERT(future_.isFinished(), return);

    Process * process = future_.result();
    QTC_ASSERT(process, return);

    static_cast<Derived *>(this)->on_process_finished_(success, process);
    process->deleteLater();

    emit finished(success);
}

} } }

#endif
