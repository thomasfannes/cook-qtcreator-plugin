#include "qook/toolset/Tool.hpp"
#include "qook/toolset/Manager.hpp"
#include <utils/environment.h>
#include <QRegularExpression>
#include <QDebug>
#include <QUuid>

namespace qook { namespace toolset {

namespace  {

const char COOK_INFO_DISPLAYNAME[] = "DisplayName";
const char COOK_INFO_ID[] = "Id";
const char COOK_INFO_CMD[] = "Cmd";
const char COOK_INFO_AUTODETECTED[] = "AutoDetected";

}

Tool::Tool()
    : d_(Detection::ManualSpecified),
      id_(generate_id())
{
}

Tool::Tool(Detection d, const Core::Id & id)
    : d_(d),
      id_(id),
      is_valid_(false)
{
    if (!id_.isValid())
        id_ = generate_id();
}

Tool * Tool::generate_from_map(const QVariantMap & map)
{
    Core::Id id = Core::Id::fromSetting(map.value(COOK_INFO_ID));

    if (!id.isValid())
        return 0;

    bool auto_detected = map.value(COOK_INFO_AUTODETECTED, false).toBool();
    Tool * tool = new Tool(auto_detected ? AutoDetected : ManualSpecified, id);

    tool->display_name_ = map.value(COOK_INFO_DISPLAYNAME).toString();
    tool->exe_file_ = QFileInfo(map.value(COOK_INFO_CMD).toString());
    tool->extract_version_info_();

    return tool;
}

bool Tool::is_default() const
{
    return id_.isValid() && Manager::instance()->default_tool_id() == id_;
}

Core::Id Tool::generate_id()
{
    return Core::Id::fromString(QUuid::createUuid().toString());
}

std::pair<Tool::Version, bool> Tool::get_version_(const QFileInfo & exec)
{
    Utils::SynchronousProcessResponse respons = run_(exec, {"-h"});
    if (respons.result == Utils::SynchronousProcessResponse::Finished)
    {
        QRegularExpression re("cook version (\\d+).(\\d+).(\\d+)");
        QRegularExpressionMatch match = re.match(respons.stdOut());

        if(match.hasMatch())
        {
            Version version;
            bool success = true;

            if (success)
                version.major = match.captured(1).toInt(&success);
            if (success)
                version.minor = match.captured(2).toInt(&success);
            if (success)
                version.patch = match.captured(3).toInt(&success);

            return std::make_pair(version, success);
        }
    }

    return std::make_pair(Version(), false);
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

std::pair<QString, bool> Tool::test_cook_executable(const QFileInfo & exec_file)
{
    std::pair<Tool::Version, bool> res = get_version_(exec_file);
    return std::make_pair(version_string_(res.first, res.second), res.second);
}

QString Tool::version_string_(const Version & version, bool is_valid)
{
    if(is_valid)
        return QString("%1.%2.%3").arg(version.major).arg(version.minor).arg(version.patch);
    else
        return QString("<invalid>");
}


QString Tool::version_string() const
{
    return version_string_(version_, is_valid_);
}

void Tool::extract_version_info_()
{
    clear_test_settings_();
    std::pair<Tool::Version, bool> respons = get_version_(exe_file_);

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
