#include "qook/cook/Tool.hpp"
#include <utils/environment.h>
#include <QRegularExpression>
#include <QUuid>

namespace qook { namespace cook {

Tool::Tool(const Core::Id &id, const QString & name)
    : name_(name),
      valid_(false),
      version_(Version()),
      run_details(RunDetails()),
      id_(id)
{
}

static Core::Id generate_id()
{
    return Core::Id::fromString(QUuid::createUuid().toString());
}


QString Tool::version_string() const
{
    if(valid_)
        return QString("%1.%2.%3").arg(version_.major).arg(version_.minor).arg(version_.patch);
    else
        return QString();
}

void Tool::extract_version_info_()
{
   Utils::SynchronousProcessResponse respons = run({"-h"});

   version_.major = 0;
   version_.minor = 0;
   version_.patch = 0;
   valid_ = false;

   if(respons.result == Utils::SynchronousProcessResponse::Finished)
   {
       QRegularExpression re("cook version: (\\d+).(\\d+).(\\d+)");
       QRegularExpressionMatch match = re.match(respons.stdOut());

       if(match.hasMatch())
       {
           bool success = true;
           if (success)
               version_.major = match.captured(1).toInt(&success);
           if (success)
               version_.minor = match.captured(2).toInt(&success);
           if (success)
               version_.patch = match.captured(3).toInt(&success);

           valid_ = success;
       }
   }
}

void Tool::set_filename(const QString & filename)
{
    exec_ = filename;
    clear_test_settings_();
    extract_version_info_();
}

Utils::SynchronousProcessResponse Tool::run(const QStringList &args) const
{
    if (!run_details.attempted_run)
    {
        QFileInfo info(exec_);
        if (!info.exists() || !info.isFile())
        {
            run_details.attempted_run = true;
            run_details.did_run = false;
        }
    }

    if (run_details.attempted_run && !run_details.did_run)
    {
        Utils::SynchronousProcessResponse response;
        response.result = Utils::SynchronousProcessResponse::StartFailed;
        return response;
    }

    Utils::SynchronousProcess cook;
    cook.setTimeoutS(1);
    cook.setFlags(Utils::SynchronousProcess::UnixTerminalDisabled);
    Utils::Environment env = Utils::Environment::systemEnvironment();
    Utils::Environment::setupEnglishOutput(&env);
    cook.setProcessEnvironment(env.toProcessEnvironment());
    cook.setTimeOutMessageBoxEnabled(false);

    Utils::SynchronousProcessResponse response = cook.runBlocking(exec_, args);
    run_details.attempted_run = true;
    run_details.did_run = (response.result == Utils::SynchronousProcessResponse::Finished);
    return response;
}

void Tool::clear_test_settings_()
{
    valid_ = false;
    run_details.attempted_run = false;
    run_details.did_run = false;
    version_ = Version();
}

} }
