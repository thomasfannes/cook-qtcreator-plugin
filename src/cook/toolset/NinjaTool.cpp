#include "cook/toolset/NinjaTool.hpp"
#include "cook/toolset/ToolManager.hpp"
#include <utils/environment.h>
#include <QRegularExpression>
#include <QDebug>
#include <QUuid>

namespace cook { namespace toolset {


NinjaTool::NinjaTool()
    : Tool(type_id())
{
}
NinjaTool::NinjaTool(Detection d, const Core::Id & id)
    : Tool(type_id(), d, id)
{
}

bool NinjaTool::is_default() const
{
    return id().isValid() && ToolManager::instance()->default_tool_id(type_id()) == id();
}

std::pair<Tool::Version, bool> NinjaTool::get_version_() const
{
    Utils::SynchronousProcessResponse respons = run_(exec_file(), {"--version"});
    if (respons.result == Utils::SynchronousProcessResponse::Finished)
    {
        QRegularExpression re("(\\d+).(\\d+).(\\d+)");
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

} }
