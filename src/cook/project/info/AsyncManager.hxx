#ifndef HEADER_cook_project_info_AsyncManager_hxx_ALREADY_INCLUDED
#define HEADER_cook_project_info_AsyncManager_hxx_ALREADY_INCLUDED

#include "cook/project/info/AsyncManager.hpp"
#include "cook/project/BuildConfiguration.hpp"
#include "cook/toolset/CookTool.hpp"
#include <QtConcurrent>
#include <coreplugin/messagemanager.h>

namespace cook { namespace project { namespace info {

namespace  {

template <typename Parser, typename OptionsFunctor>
auto run(const BuildConfiguration * config, OptionsFunctor get_options)
{
    using Mgr = AsyncManager<Parser>;
    using Request = typename Mgr::Request;

    Request result;
    // try the tool
    const toolset::CookTool * tool = config->tool();
    if(tool == nullptr || !tool->is_valid())
    {
        result.code = Mgr::ToolFailed;
        result.error = "The specified Cook tool is not valid. Please update the kit";
        return result;
    }

    QStringList options = get_options(config);
    QString cmd = QString("[cook]: %1 %2").arg(tool->user_file_name()).arg(options.join(" "));
    Core::MessageManager::write(cmd, Core::MessageManager::Silent);

    // execute the tool
    Utils::SynchronousProcessResponse respons = tool->run(options);
    Core::MessageManager::write(respons.allOutput(), Core::MessageManager::Silent);

    if(respons.exitCode != Utils::SynchronousProcessResponse::Finished)
    {
        result.code = Mgr::ToolFailed;
        result.error = QString("The Cook tool failed:\n%1").arg(respons.allOutput());

        Core::MessageManager::write(result.error, Core::MessageManager::Silent);
        return result;
    }

    Parser p;
    QObject::connect(&p, &Parser::error_occured, [&](QString error) { result.error.append(error); });

    // try the parsing
    if (!p.parse(result.result, respons.rawStdOut))
    {
        result.code = Mgr::ParsingFailed;
        result.error.append(QString("\n%1").arg(respons.allOutput()));

        Core::MessageManager::write(result.error, Core::MessageManager::Silent);
        return result;
    }


    result.code = Mgr::Ok;
    return result;
}

}

template <typename Parser>
template <typename OptionsFunctor>
bool AsyncManager<Parser>::async_run_(const OptionsFunctor & get_options)
{
    if (!future_.isFinished())
        return false;

    future_ = QtConcurrent::run([=]() { return run<Parser>(config_, get_options); });
    watcher_.setFuture(future_);

    return true;
}

} } }

#endif

