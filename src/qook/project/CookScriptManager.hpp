#ifndef HEADER_qook_project_CookScriptManager_hpp_ALREADY_INCLUDED
#define HEADER_qook_project_CookScriptManager_hpp_ALREADY_INCLUDED

#include "qook/project/RecipeScanner.hpp"
#include "qook/project/Structure.hpp"

#include <QFuture>
#include <QFutureWatcher>

namespace qook { namespace project {

class BuildConfiguration;

class CookScriptManager : public QObject
{
    Q_OBJECT

public:
    CookScriptManager(BuildConfiguration * configuration);

    bool start_async_parse();
    const Cook & last() const { return last_; }

    QFuture<void> future();

signals:
    void started();
    void error_occured(QString error);
    void data_available();



private slots:
    void finished();

private:
    using Future = QFuture<ScanResult>;
    using FutureWatcher = QFutureWatcher<ScanResult>;

    Future future_;
    FutureWatcher watcher_;
    BuildConfiguration * config_;
    Cook last_;
};

} }

#endif
