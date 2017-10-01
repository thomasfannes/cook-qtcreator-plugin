#ifndef HEADER_cook_project_BuildNinjaManager_hpp_ALREADY_INCLUDED
#define HEADER_cook_project_BuildNinjaManager_hpp_ALREADY_INCLUDED

#include <QFuture>
#include <QFutureWatcher>

namespace cook { namespace project {

class BuildConfiguration;

class BuildNinjaManager : public QObject
{
    using Future = QFuture<QString>;
    using Watcher = QFutureWatcher<QString>;

    Q_OBJECT

public:
    BuildNinjaManager(BuildConfiguration * config);

    bool start_async();
    QFuture<void> future() const;

signals:
    void started();
    void error_occured(QString error);
    void finished(bool);

private:
    void run_finished_();

    Future future_;
    Watcher watcher_;
    BuildConfiguration * config_;
};

} }

#endif
