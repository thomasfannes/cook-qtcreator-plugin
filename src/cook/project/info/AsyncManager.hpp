#ifndef HEADER_cook_project_info_AsyncManager_hpp_ALREADY_INCLUDED
#define HEADER_cook_project_info_AsyncManager_hpp_ALREADY_INCLUDED

#include <QString>
#include <QFuture>
#include <QFutureWatcher>

namespace cook { namespace project {

class BuildConfiguration;

namespace info {

class AsyncManagerBase : public QObject
{
    Q_OBJECT

signals:
    void started();
    void error_occured(QString error);
    void finished(bool);
};


template <typename Parser>
class AsyncManager : public AsyncManagerBase
{
public:
    enum ResultCode { Ok, NotProcessed, ToolFailed, ParsingFailed };

    using value_type = typename Parser::value_type;

    struct Request
    {
        ResultCode code = NotProcessed;
        typename Parser::value_type result;
        QString error;
    };

    explicit AsyncManager(const BuildConfiguration *config)
        : config_(config)
    {
        connect(&watcher_, &Watcher::finished, this, &AsyncManager<Parser>::run_finished);
        connect(&watcher_, &Watcher::started, this, &AsyncManager<Parser>::started);
    }

    QFuture<void> future() const            { return future_; }
    const value_type & latest() const       { return value_; }

protected:
    template <typename OptionsFunctor> bool async_run_(const OptionsFunctor & get_options);

private:
    void run_finished()
    {
        if (!future_.isFinished())
            return;

        Request req = future_.result();
        if(req.code != Ok)
            emit error_occured(req.error);
        else
            value_ = std::move(req.result);

        emit finished(req.code == Ok);
    }

private:
    using Future = QFuture<Request>;
    using Watcher = QFutureWatcher<Request>;

    value_type value_;
    Future future_;
    Watcher watcher_;

    const BuildConfiguration * config_;
};





} } }

#endif



