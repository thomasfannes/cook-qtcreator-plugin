#include "qook/toolset/CookTool.hpp"
#include "qook/toolset/Manager.hpp"
#include <utils/environment.h>
#include <QRegularExpression>
#include <QDebug>
#include <QUuid>

namespace qook { namespace toolset {

CookTool::CookTool()
    : Tool(type_id())
{
}
CookTool::CookTool(Detection d, const Core::Id & id)
    : Tool(type_id(), d, id)
{
}

bool CookTool::is_default() const
{
    return id().isValid() && Manager::instance()->default_tool_id(type_id()) == id();
}

std::pair<CookTool::Version, bool> CookTool::get_version_() const
{
    Utils::SynchronousProcessResponse respons = run_(exec_file(), {"-h"});
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

} }
