#ifndef HEADER_cook_project_info_CookProcess_hpp_ALREADY_INCLUDED
#define HEADER_cook_project_info_CookProcess_hpp_ALREADY_INCLUDED

#include <utils/qtcprocess.h>

namespace cook { namespace toolset {

class CookTool;

} }

namespace cook { namespace project { namespace info {

class CookProcess : public QProcess
{
    Q_OBJECT

public:
    explicit CookProcess(const toolset::CookTool * tool, QObject * parent = nullptr);

    bool valid() const;

protected:
    bool start_blocking_(const QStringList & arguments, int msecs = 3000);

signals:
    void started(const QString & command, const QStringList & arguments);
    void standard_out_written(const QByteArray & ar_out);
    void standard_error_written(const QByteArray & ar_err);
    void error_occured(const QString & error);
    void warning_occured(const QString & error);
    void finished(bool);

private:
    void standard_output_ready_();
    void standard_error_ready_();
    void on_started_();

    const toolset::CookTool * tool_;

};

} } }

#endif
