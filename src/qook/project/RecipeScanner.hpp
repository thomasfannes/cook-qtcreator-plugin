#ifndef HEADER_qook_project_RecipeScanner_hpp_ALREADY_INCLUDED
#define HEADER_qook_project_RecipeScanner_hpp_ALREADY_INCLUDED

#include "qook/project/Structure.hpp"
#include "qook/toolset/Tool.hpp"

#include <utils/fileutils.h>
#include <QObject>
#include <QFuture>
#include <QFutureWatcher>

namespace qook { namespace project {

struct ScanResult
{
    enum ResultCode
    {
        Ok,
        NotProcessed,
        ToolFailed,
        ParsingFailed
    };

    ResultCode result_code;
    Cook cook;
    QString error;
    QString output;
};

class RecipeScanRun : public QObject
{
    Q_OBJECT

public:
    RecipeScanRun(const toolset::Tool * tool, const QStringList &extra_options);

    bool operator()();
    const ScanResult & result() const { return value_; }

private slots:
    void parsing_error(const QString & error);

private:
    const toolset::Tool * tool_;
    ScanResult value_;
    QStringList options_;
};


//class RecipeScanner : public QObject
//{
//    Q_OBJECT

//public:
//    using Result = ScanResult;
//    using Future = QFuture<Result>;
//    using FutureWatcher = QFutureWatcher<Result>;

//    explicit RecipeScanner(QObject * parent = nullptr);

//    bool async_scan_structure(const toolset::Tool * tool, const Utils::FileName & directory);

//    Future future() const;
//    bool is_finished() const;


//    ScanResult * last_succesful() const { return last_result_; }
//    ScanResult * last_result() const;

//signals:
//    void started();
//    void finished(bool);
//    void error_occured(ScanResult::ResultCode code, QString error);

//private slots:
//    void scan_finished();



//private:
//    static Result scan_structure_(const toolset::Tool * tool, const Utils::FileName & directory);

//    FutureWatcher future_watcher_;
//    Future future_;

//    Cook last_result_;

//    ScanResult * last_result_;
//};

} }

#endif
