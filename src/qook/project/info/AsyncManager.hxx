#ifndef HEADER_qook_project_info_AsyncManager_hxx_ALREADY_INCLUDED
#define HEADER_qook_project_info_AsyncManager_hxx_ALREADY_INCLUDED

#include "qook/project/info/AsyncManager.hpp"
#include "qook/project/BuildConfiguration.hpp"
#include "qook/toolset/CookTool.hpp"
#include <QtConcurrent>
#include <coreplugin/messagemanager.h>

namespace qook { namespace project { namespace info {

template <typename Parser>
template <typename OptionsFunctor>
bool AsyncManager<Parser>::async_run_(const OptionsFunctor & get_options)
{
    if (!future_.isFinished())
        return false;

    future_ = QtConcurrent::run([this](OptionsFunctor get_options)
    {
        Request result;
        // try the tool
        const toolset::CookTool * tool = config_->tool();
        if(tool == nullptr || !tool->is_valid())
        {
            result.code = ToolFailed;
            result.error = "The specified Cook tool is not valid. Please update the kit";
            return result;
        }

        QStringList options = get_options(config_);
        QString cmd = QString("[cook]: %1 %2").arg(tool->user_file_name()).arg(options.join(" "));
        Core::MessageManager::write(cmd, Core::MessageManager::Silent);

        // execute the tool
        Utils::SynchronousProcessResponse respons = tool->run(get_options(config_));
        if(respons.exitCode != Utils::SynchronousProcessResponse::Finished)
        {
            result.code = ToolFailed;
            result.error = QString("The Cook tool failed:\n%1").arg(respons.allOutput());
            return result;
        }

        Parser p;
        QObject::connect(&p, &Parser::error_occured, [&](QString error) { result.error.append(error); });

        // try the parsing
        if (!p.parse(result.info, respons.rawStdOut))
        {
            result.code = ParsingFailed;
            result.error.append(QString("\n%1").arg(respons.allOutput()));
            return result;
        }

        result.code = Ok;
        return result;
    }, get_options);

    watcher_.setFuture(future_);

    return true;
}

} } }

#endif

