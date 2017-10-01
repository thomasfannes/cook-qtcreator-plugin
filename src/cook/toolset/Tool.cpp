#include "cook/toolset/Tool.hpp"
#include <utils/environment.h>
#include <QRegularExpression>
#include <QDebug>
#include <QUuid>

namespace cook { namespace toolset {

namespace  {

const char COOK_INFO_DISPLAYNAME[] = "DisplayName";
const char COOK_INFO_ID[] = "Id";
const char COOK_INFO_CMD[] = "Cmd";
const char COOK_INFO_AUTODETECTED[] = "AutoDetected";

}

Tool::Tool(const Core::Id &type_id)
    : d_(Detection::ManualSpecified),
      type_id_(type_id),
      id_(generate_id()),
      is_valid_(false)
{
}

Tool::Tool(const Core::Id & type_id, Detection d, const Core::Id & id)
    : d_(d),
      type_id_(type_id),
      id_(id),
      is_valid_(false)
{
    if (!id_.isValid())
        id_ = generate_id();
}


Core::Id Tool::generate_id()
{
    return Core::Id::fromString(QUuid::createUuid().toString());
}

bool Tool::from_map(const QVariantMap & map)
{
    id_ = Core::Id::fromSetting(map.value(COOK_INFO_ID));
    if (!id_.isValid())
        return false;

    d_ = map.value(COOK_INFO_AUTODETECTED, false).toBool() ? AutoDetected : ManualSpecified;
    display_name_ = map.value(COOK_INFO_DISPLAYNAME).toString();
    exe_file_ = QFileInfo(map.value(COOK_INFO_CMD).toString());

    extract_version_info_();
    return true;
}

QVariantMap Tool::to_map() const
{
    QVariantMap data;
    data.insert(COOK_INFO_DISPLAYNAME, display_name_);
    data.insert(COOK_INFO_ID, id_.toSetting());
    data.insert(COOK_INFO_CMD, exe_file_.absoluteFilePath());
    data.insert(COOK_INFO_AUTODETECTED, is_auto_detected());

    return data;
}

QString Tool::version_string() const
{
    if(is_valid_)
        return QString("%1.%2.%3").arg(version_.major).arg(version_.minor).arg(version_.patch);
    else
        return QString("<invalid>");
}

void Tool::extract_version_info_()
{
    clear_test_settings_();
    std::pair<Tool::Version, bool> respons = get_version_();

    version_ = respons.first;
    is_valid_ = respons.second;
}

void Tool::set_display_name(const QString & display_name)
{
    display_name_ = display_name;
}

void Tool::set_exec_file(const QFileInfo & exec_file)
{
    exe_file_ = exec_file;
    clear_test_settings_();
    extract_version_info_();
}

Utils::SynchronousProcessResponse Tool::run_(const QFileInfo & exec, const QStringList &args)
{
    Utils::SynchronousProcess process;
    process.setTimeoutS(1);
    process.setFlags(Utils::SynchronousProcess::UnixTerminalDisabled);
    Utils::Environment env = Utils::Environment::systemEnvironment();
    Utils::Environment::setupEnglishOutput(&env);
    process.setProcessEnvironment(env.toProcessEnvironment());
    process.setTimeOutMessageBoxEnabled(false);

    Utils::SynchronousProcessResponse response = process.runBlocking(exec.absoluteFilePath(), args);
    return response;
}

Utils::SynchronousProcessResponse Tool::run(const QStringList &args) const
{
    if (!run_details.attempted_run)
    {
        if(!exe_file_.exists() || !exe_file_.isFile() || !exe_file_.isExecutable())
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

    Utils::SynchronousProcessResponse response = run_(exe_file_, args);
    run_details.attempted_run = true;
    run_details.did_run = (response.result == Utils::SynchronousProcessResponse::Finished);
    return response;
}

void Tool::clear_test_settings_()
{
    is_valid_ = false;
    run_details.attempted_run = false;
    run_details.did_run = false;
    version_ = Version();
}

} }
