#include "qook/toolset/Manager.hpp"
#include "qook/toolset/Tool.hpp"

#include <utils/environment.h>
#include <utils/hostosinfo.h>
#include <utils/algorithm.h>
#include <utils/qtcassert.h>
#include <utils/persistentsettings.h>
#include <coreplugin/icore.h>

#include <QDebug>

namespace qook { namespace toolset {

namespace {

const char COOK_TOOL_FILENAME[] = "/qtcreator/cooktools.xml";
const char COOK_TOOL_COUNT_KEY[] = "CookTools.Count";
const char COOK_TOOL_DEFAULT_KEY[] = "CookTools.Default";
const char COOK_TOOL_DATA_KEY[] = "CookTools.";
const char COOK_TOOL_FILE_VERSION_KEY[] = "Version";

Utils::FileName user_settings_filename()
{
    QFileInfo settingsLocation(Core::ICore::settings()->fileName());
    return Utils::FileName::fromString(settingsLocation.absolutePath() + QLatin1String(COOK_TOOL_FILENAME));
}


}

Manager * Manager::instance_ = nullptr;

Manager::Manager(QObject *parent)
    : QObject(parent),
      default_tool_id_()
{
    writer_ = new Utils::PersistentSettingsWriter(user_settings_filename(), QStringLiteral("QtCreatorCookTools"));

    connect(this, &Manager::tool_added, this, &Manager::tools_changed);
    connect(this, &Manager::tool_removed, this, &Manager::tools_changed);
    connect(this, &Manager::tool_updated, this, &Manager::tools_changed);
    connect(Core::ICore::instance(), &Core::ICore::saveSettingsRequested, this, &Manager::save_tools_);
}

Manager::~Manager()
{
    qDeleteAll(registered_tools_);
    delete writer_;
}

Manager * Manager::create_instance(QObject * parent)
{
    if(instance_ != nullptr)
        return nullptr;

    instance_ = new Manager(parent);
    return instance_;
}

Manager * Manager::instance()
{
    return instance_;
}

QList<const Tool *> Manager::registered_tools() const
{
    QList<const Tool *> lst;
    for(Tool * t : registered_tools_)
        lst.append(t);

    return lst;
}

bool Manager::register_tool(Tool * tool)
{
    // an existing, new tool?
    if (!tool || registered_tools_.contains(tool))
        return false;

    // the id should be valid
    QTC_ASSERT(tool->id().isValid(), return false);

    // do we have a tool with the same id?
    for(Tool * t : registered_tools_)
        if (t->id() == tool->id())
            return false;

    registered_tools_.append(tool);
    if (!default_tool_id_.isValid())
    {
        default_tool_id_ = tool->id();
        emit default_tool_changed();
    }
    emit tool_added(tool->id());

    return true;
}

const Tool * Manager::update_or_register_tool(const Tool & update)
{
    Tool * tool = nullptr;

    auto it = std::find_if(registered_tools_.begin(), registered_tools_.end(), [&](Tool * t) {return t->id() == update.id();});
    if (it == registered_tools_.end())
    {
        // add a new tool
        tool = new Tool(update);
        QTC_ASSERT(register_tool(tool), return nullptr);
    }
    else
    {
        tool = *it;
        tool->set_display_name(update.display_name());
        tool->set_exec_file(update.exec_file());
    }

    emit tool_updated(tool->id());

    return tool;
}

const Tool *Manager::find_registered_tool(const Core::Id & id) const
{
    auto it = std::find_if(registered_tools_.begin(), registered_tools_.end(), [&](Tool * t) {return t->id() == id;});
    return it != registered_tools_.end() ? *it : nullptr;
}

void Manager::deregister_tool(const Core::Id &id)
{
    auto it = std::find_if(registered_tools_.begin(), registered_tools_.end(), [&](Tool * t) { return t->id() == id; });
    if (it != registered_tools_.end())
    {
        // take and remove the tool
        Tool * t = *it;
        registered_tools_.erase(it);

        // the default tool
        if (t->id() == default_tool_id_)
        {
            if (registered_tools_.empty())
                default_tool_id_ = Core::Id();
            else
                default_tool_id_ = registered_tools_.front()->id();

            emit default_tool_changed();
        }

        emit tool_removed(id);
        delete t;
    }
}

void Manager::set_default_tool(const Core::Id &id)
{
    if(id == default_tool_id_)
        return;

    auto it = std::find_if(registered_tools_.begin(), registered_tools_.end(), [&](Tool * t) { return t->id() == id; });
    if (it != registered_tools_.end())
    {
        default_tool_id_ = id;
        emit default_tool_changed();
    }
}


void Manager::restore()
{
    // find all tools
    QList<Tool*> persistent = load_tools_();
    QList<Tool*> auto_detected = autodetect_tools_();

    QList<Tool*> to_register;
    // start by adding the persistent tools
    while(!persistent.empty())
    {
        Tool * tool = persistent.takeLast();

        // an auto detected tool not in the auto_detected list, is from a previous setting
        // the user cannot edit it, so do not add it
        if (tool->is_auto_detected())
        {
            // not in the autodetected list?
            if (!Utils::anyOf(auto_detected, Utils::equal(&Tool::exec_file, tool->exec_file())))
            {
                qWarning() << QString::fromLatin1("Previously autodetected cook tool \"%1\" (%2) dropped.").arg(tool->exec_file().absoluteFilePath(), tool->id().toString());

                delete tool;
                continue;
            }
        }

        to_register.append(tool);
    }

    // now consider the auto_detected
    while(!auto_detected.empty())
    {
        Tool * tool = auto_detected.takeLast();

        // is an autodetected in the to_register list?
        if(Utils::anyOf(to_register, [=](Tool * t) { return t->is_auto_detected() && t->exec_file() == tool->exec_file(); } ))
        {
            delete tool;
            continue;
        }

        to_register.append(tool);
    }

    for(Tool * tool : to_register)
        if(!register_tool(tool))
        {
            qWarning() << QString::fromLatin1("cook tool \"%1\" (%2) dropped.").arg(tool->exec_file().absoluteFilePath(), tool->id().toString());
            delete tool;
        }
}

QList<Tool *> Manager::load_tools_()
{
    Utils::PersistentSettingsReader reader;
    if(!reader.load(user_settings_filename()))
        return {};

    QVariantMap data = reader.restoreValues();
    int version = data.value(QLatin1String(COOK_TOOL_FILE_VERSION_KEY)).toInt();
    if (version != 1)
        return {};

    QList<Tool*> loaded;
    unsigned int count = data.value(QLatin1String(COOK_TOOL_COUNT_KEY),0).toUInt();
    for(unsigned int i = 0; i < count; ++i)
    {
        const QString & key = QString::fromLatin1(COOK_TOOL_DATA_KEY) + QString::number(i);
        if(!data.contains(key))
            continue;

        const QVariantMap dbMap = data.value(key).toMap();
        Tool * tool = Tool::generate_from_map(dbMap);
        if (!tool)
            continue;

        if (tool->is_auto_detected() && !tool->is_valid())
            continue;

        loaded.append(tool);
    }

    default_tool_id_ = Core::Id::fromSetting(data.value(QLatin1String(COOK_TOOL_DEFAULT_KEY), default_tool_id_.toSetting()));

    return loaded;
}

void Manager::save_tools_()
{
    QTC_ASSERT(writer_, return);

    QVariantMap data;
    data.insert(QLatin1String(COOK_TOOL_FILE_VERSION_KEY), 1);
    data.insert(QLatin1String(COOK_TOOL_DEFAULT_KEY), default_tool_id().toString());

    unsigned int count = 0;
    for(const Tool  * tool : registered_tools_)
    {
        // only same those that can be executed
        if(tool->is_valid())
        {
            QVariantMap map = tool->to_map();
            if(map.isEmpty())
                continue;

            data.insert(QString::fromLatin1(COOK_TOOL_DATA_KEY) + QString::number(count), map);
            ++count;
        }
    }

    data.insert(QLatin1String(COOK_TOOL_COUNT_KEY), count);
    writer_->save(data, Core::ICore::mainWindow());
}

QList<Tool *> Manager::autodetect_tools_()
{
    QSet<QString> candidates;

    Utils::Environment env = Utils::Environment::systemEnvironment();
    QStringList path = env.path();

    // additional search path for windows
    if (Utils::HostOsInfo::isWindowsHost())
    {
        path.append("C:/Program Files/CMake");
        path.append("C:/Program Files (x86)/CMake");
    }

    // additional search path for macOS
    if (Utils::HostOsInfo::isMacHost())
    {
        path.append("/Applications/CMake.app/Contents/bin");
        path.append("/usr/local/bin");
        path.append("/opt/local/bin");
    }

    // get the possible names for the executable
    QStringList execs = env.appendExeExtensions(QLatin1String("cook"));

    // loop over all paths
    for(QString base : path)
    {
        const QChar slash = QLatin1Char('/');
        if(base.isEmpty()) continue;

        // no double slash for windows
        if (!base.endsWith(slash)) base += slash;

        // try every executable at that location
        for(const QString & exec : execs)
        {
            // get a possible executable candidate
            QFileInfo fi(base + exec);
            if(fi.exists() && fi.isFile() && fi.isExecutable())
                candidates << fi.absoluteFilePath();
        }
    }

    // for every candidate
    QList<Tool *> auto_detected;
    for(const QString & candidate : candidates)
    {
        QFileInfo info(candidate);

        // generate the tool
        auto tool = new Tool(Tool::AutoDetected, Tool::generate_id());
        tool->set_exec_file(info);
        tool->set_display_name( QString("System cook at %1").arg(candidate));

        // only valid tools should be in the list
        if (!tool->is_valid())
            delete tool;
        else
            auto_detected << tool;
    }

    return auto_detected;
}

} }
