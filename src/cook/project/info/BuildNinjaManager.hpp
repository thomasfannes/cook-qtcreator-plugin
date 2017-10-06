#ifndef HEADER_cook_project_info_BuildNinjaManager_hpp_ALREADY_INCLUDED
#define HEADER_cook_project_info_BuildNinjaManager_hpp_ALREADY_INCLUDED

#include <QFuture>
#include <QFutureWatcher>
#include <QObject>

namespace cook { namespace project {

class BuildConfiguration;

namespace info {

class BuildNinjaManager : public QObject
{
    Q_OBJECT

public:
    BuildNinjaManager(const BuildConfiguration *config);

    bool start_async();
    QFuture<void> future() const            { return future_; }

signals:
    void started();
    void error_occured(QString error);
    void finished(bool);

private:
    void run_finished();

    using Future = QFuture<QString>;
    using Watcher = QFutureWatcher<QString>;

    Future future_;
    Watcher watcher_;
    const BuildConfiguration *config_;

};

} } }

#endif
