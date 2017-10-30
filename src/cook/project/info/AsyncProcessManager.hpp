#ifndef HEADER_cook_project_info_AsyncProcessManager_hpp_ALREADY_INCLUDED
#define HEADER_cook_project_info_AsyncProcessManager_hpp_ALREADY_INCLUDED

#include <QFuture>
#include <QString>

namespace cook { namespace project {

class BuildConfiguration;

} }

namespace cook { namespace project { namespace info {

class AsyncProcessManagerBase : public QObject
{
    Q_OBJECT

signals:
    void started(const QString & command, const QStringList & arguments);
    void process_output(const QByteArray & process_output);
    void finished(bool);
    void error_occured(QString error);
    void warning_occured(QString warning);
};

template <typename Derived, typename Process>
class AsyncProcessManager : public AsyncProcessManagerBase
{
public:
    AsyncProcessManager(const BuildConfiguration * config);
    ~AsyncProcessManager();

    bool start_async();
    QFuture<void> future() const { return future_; }

private:
    void on_finished_(bool);
    void on_process_finished_(bool success, const Process * process) {}

    const BuildConfiguration * config_;
    QFuture<Process*> future_;
};

} } }

#include "cook/project/info/AsyncProcessManager.hxx"

#endif
