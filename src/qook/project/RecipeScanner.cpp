#include "qook/project/RecipeScanner.hpp"
#include "qook/project/Parser.hpp"
#include <utils/runextensions.h>

#include <QtConcurrent>

namespace qook { namespace project {

RecipeScanRun::RecipeScanRun(const toolset::Tool * tool, const QStringList & options)
    : tool_(tool),
      options_(options)
{
    value_.result_code = ScanResult::NotProcessed;

    if(!tool_)
    {
        value_.result_code = ScanResult::ToolFailed;
        value_.error = QString("Invalid tool: null pointer");
    }
}

bool RecipeScanRun::operator()()
{
    // only process once
    if (value_.result_code != ScanResult::NotProcessed)
        return false;

    Parser p;
    connect(&p, &Parser::error_occurred, this, &RecipeScanRun::parsing_error);

    Utils::SynchronousProcessResponse response = tool_->run(options_);
    value_.output = response.stdOut();

    if (response.result != Utils::SynchronousProcessResponse::Finished)
    {
        value_.result_code = ScanResult::ToolFailed;
        value_.error = response.allOutput();
        return false;
    }

    if(!p.parse(value_.cook, response.rawStdOut))
    {
        value_.result_code = ScanResult::ParsingFailed;
        return false;
    }

    value_.result_code = ScanResult::Ok;
    return true;
}

void RecipeScanRun::parsing_error(const QString & error)
{
    value_.error.append(QString("Parsing error: %1").arg(error));
}


//RecipeScanner::RecipeScanner(QObject * parent)
//    : QObject(parent),
//      last_result_(nullptr)
//{
//    connect(&future_watcher_, &FutureWatcher::finished, this, &RecipeScanner::scan_finished);
//}

//void RecipeScanner::scan_finished()
//{
//    // handle the result
//    ScanResult * current_result = new ScanResult(future().result());

//    if(current_result->result_code == ScanResult::Ok)
//    {
//        delete last_result_;
//        last_result_ = current_result;

//        emit finished(true);
//    }
//    else
//    {

//    }

//    const auto & result =

//    emit finished(future().result());

//    void finished(bool);
//    void error_occured(ScanResult::ResultCode code, QString error);
//}

//bool RecipeScanner::async_scan_structure(const toolset::Tool * tool, const Utils::FileName & directory)
//{
//    if (!future_.isFinished())
//        return false;

//    emit started();

//    future_ = QtConcurrent::run(&RecipeScanner::scan_structure_, tool, directory);
//    future_watcher_.setFuture(future_);
//    return true;
//}

//RecipeScanner::Future RecipeScanner::future() const
//{
//    return future_;
//}

//bool RecipeScanner::is_finished() const
//{
//    return future_.isStarted() && future_.isFinished();
//}

//RecipeScanner::Result RecipeScanner::result() const
//{
//    if(is_finished())
//        return future_.result();

//    ScanResult result;
//    result.result_code = ScanResult::NotProcessed;

//    return result;
//}


//RecipeScanner::Result RecipeScanner::scan_structure_(const toolset::Tool * tool, const Utils::FileName & directory)
//{
//    RecipeScanRun run(tool);
//    run(directory);

//    return run.result();
//}

} }
