#include "cook/project/info/CookProcess.hpp"
#include "cook/toolset/CookTool.hpp"

namespace cook { namespace project { namespace info {

CookProcess::CookProcess(const toolset::CookTool * tool, QObject *parent)
    : QProcess(parent),
      tool_(tool)
{
    connect(this, &QProcess::readyReadStandardError,    this, &CookProcess::standard_error_ready_);
    connect(this, &QProcess::readyReadStandardOutput,   this, &CookProcess::standard_output_ready_);
    connect(this, &QProcess::started,                   this, &CookProcess::on_started_);
}

bool CookProcess::valid() const
{
    return tool_ && tool_->is_valid();
}

bool CookProcess::start_blocking_(const QStringList & arguments, int msecs)
{
    if (!valid())
    {
        emit error_occured("No valid Cook tool specified. Please configure the kit.");
        return false;
    }

    // set and run
    setProgram(tool_->exec_file().absoluteFilePath());
    setArguments(arguments);
    start();

    // make sure the process has started
    {
        const bool ok = waitForStarted();
        if (!ok)
        {
            switch(error())
            {
                case QProcess::Timedout:
                    emit error_occured("Failed to start: Timed out");
                    break;

                default:
                    emit error_occured("Failed to start");
            }

            return false;
        }
    }

    bool was_succesful = true;

    // wait for finished
    {
        const bool ok = waitForFinished(msecs);

        // close the process
        close();

        if (!ok)
        {
            switch(error())
            {
                case QProcess::Crashed:
                    emit error_occured("Failed to run: Process crashed");
                    break;

                case QProcess::Timedout:
                    emit error_occured("Failed to run: Timed out");
                    break;

                default:
                    emit error_occured("Failed to run");
            }

            was_succesful = false;
        }
    }

    // process finished, did it finish correclty?
    was_succesful = was_succesful && exitStatus() == QProcess::NormalExit && exitCode() == 0;
    emit finished(was_succesful);

    return was_succesful;
}

void CookProcess::on_started_()
{
    emit started(program(), arguments());
}

void CookProcess::standard_output_ready_()
{
    QByteArray array = this->readAllStandardOutput();
    emit standard_out_written(array);
}

void CookProcess::standard_error_ready_()
{
    QByteArray array = this->readAllStandardError();
    emit standard_error_written(array);
}

} } }
