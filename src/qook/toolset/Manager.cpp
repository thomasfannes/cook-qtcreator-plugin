#include "qook/toolset/Manager.hpp"
#include "qook/toolset/Tool.hpp"
#include <utils/environment.h>
#include <utils/hostosinfo.h>
#include <utils/algorithm.h>
#include <utils/qtcassert.h>
#include <utils/persistentsettings.h>
#include <utils/fileutils.h>
#include <coreplugin/icore.h>
#include <QFileInfo>
#include <QDebug>

namespace qook { namespace toolset {

namespace {

const char COOK_TOOL_FILENAME[] = "/qtcreator/cooktools.xml";
const char COOK_TOOL_COUNT_KEY[] = "CookTools.Count";

const char COOK_TOOL_DEFAULT_COUNT[] = "CookTools.Default.Count";
const char COOK_TOOL_DEFAULT_KEY[] = "CookTools.Default.Type.";
const char COOK_TOOL_DEFAULT_VALUE[] = "CookTools.Default.Id.";

const char COOK_TOOL_DATA_KEY[] = "CookTools.";
const char COOK_TOOL_TYPE_ID[] = "CookTools.Id";
const char COOK_TOOL_FILE_VERSION_KEY[] = "Version";

}

Manager * Manager::instance_ = nullptr;

Manager::Manager(QObject *parent)
    : QObject(parent),
      writer_(nullptr)
{
    connect(this, &Manager::tool_added, this, &Manager::tools_changed);
    connect(this, &Manager::tool_removed, this, &Manager::tools_changed);
    connect(this, &Manager::tool_updated, this, &Manager::tools_changed);
    connect(Core::ICore::instance(), &Core::ICore::saveSettingsRequested, this, &Manager::save_tools_);
}


void Manager::initialize_writer_()
{
    if(!writer_)
        writer_ = new Utils::PersistentSettingsWriter(user_settings_filename(), QStringLiteral("QtCreatorCookTools"));
}


Manager * Manager::create_instance(QObject * parent)
{
    QTC_ASSERT(!instance_, return instance_);

    instance_ = new Manager(parent);
    return instance_;
}


Manager * Manager::instance()
{
    QTC_CHECK(instance_);
    return instance_;
}


Manager::~Manager()
{
    qDeleteAll(registered_tools_);
    delete writer_;

    qDeleteAll(factories_);
}


Utils::FileName Manager::user_settings_filename()
{
    QFileInfo settingsLocation(Core::ICore::settings()->fileName());
    return Utils::FileName::fromString(settingsLocation.absolutePath() + QString("/qtcreator/cook/tools.xml"));
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

    Core::Id & def_id = default_tools_[tool->type_id()];
    if (!def_id.isValid())
    {
        def_id = tool->id();
        emit default_tool_changed();
    }
    emit tool_added(tool->id(), tool->type_id());

    return true;
}


const Tool * Manager::update_or_register_tool(const Tool & update)
{
    Tool * tool = nullptr;

    auto it = std::find_if(registered_tools_.begin(), registered_tools_.end(), [&](Tool * t) {return t->id() == update.id();});
    if (it == registered_tools_.end())
    {
        Factory * factory = factories_.value(update.type_id());
        QTC_ASSERT(factory, return nullptr);

        // add a new tool
        tool = factory->construct(update);
        QTC_ASSERT(register_tool(tool), return nullptr);
    }
    else
    {
        tool = *it;
        tool->set_display_name(update.display_name());
        tool->set_exec_file(update.exec_file());
    }

    emit tool_updated(tool->id(), tool->type_id());

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

        // did we remove the default tool
        {
            const Core::Id & type_id = t->type_id();
            if(default_tool_id(type_id) == t->id())
            {
                const Tool * tool = Utils::findOr(registered_tools(), nullptr, Utils::equal(&Tool::type_id, type_id));
                default_tools_[type_id] = (tool ? tool->id() : Core::Id());
                emit default_tool_changed();
            }
        }

        emit tool_removed(id, t->type_id());
        delete t;
    }
}


void Manager::set_default_tool(const Core::Id &id)
{
    auto it = std::find_if(registered_tools_.begin(), registered_tools_.end(), [&](Tool * t) { return t->id() == id; });
    if (it == registered_tools_.end())
        return;

    const Tool * tool = *it;
    Core::Id & def_id = default_tools_[tool->type_id()];

    if(id == def_id)
        return;

    def_id = id;
    emit default_tool_changed();
}


void Manager::restore()
{
    // find all tools
    QList<Tool*> persistent = load_tools_();

    QList<Tool*> auto_detected;
    for(const Factory * factory : factories_)
    {
        QTC_ASSERT(factory, continue);
        auto_detected.append(autodetect_tools_(*factory));
    }

    QList<Tool*> to_register;


    // start by adding the persistent tools
    while(!persistent.empty())
    {
        Tool * tool = persistent.takeLast();

        // a persisten auto-detected tool which is not valid should not be added
        if (tool->is_auto_detected() && !tool->is_valid())
        {
            qWarning() << QString::fromLatin1("Previously autodetected %1 tool \"%2\" (%3) dropped.").arg(tool_name_).arg(tool->exec_file().absoluteFilePath(), tool->id().toString());
            delete tool;
            continue;
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
    {
        if(!register_tool(tool))
        {
            qWarning() << QString::fromLatin1("%1 tool \"%1\" (%2) dropped.").arg(tool_name_).arg(tool->exec_file().absoluteFilePath(), tool->id().toString());
            delete tool;
        }
    }

    for(auto it = default_tools_.begin(); it != default_tools_.end(); ++it)
    {
        const Core::Id type_id = it.key();
        Core::Id & id = *it;

        if (!Utils::anyOf(registered_tools(), Utils::equal(&Tool::id, id)))
        {
            const Tool * tool = Utils::findOr(registered_tools(), nullptr, Utils::equal(&Tool::type_id, type_id));
            id = tool ? tool->id() : Core::Id();
        }
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
        const QString & type_key = QString::fromLatin1(COOK_TOOL_TYPE_ID) + QString::number(i);
        if(!data.contains(type_key))
            continue;

        const Core::Id id = Core::Id::fromSetting(data.value(type_key));
        const Factory * factory = factories_.value(id);
        if(!factory)
            continue;

        const QString & key = QString::fromLatin1(COOK_TOOL_DATA_KEY) + QString::number(i);
        if(!data.contains(key))
            continue;

        const QVariantMap dbMap = data.value(key).toMap();
        {
            Tool * tool = factory->construct();
            if (!tool->from_map(dbMap) || (tool->is_auto_detected() && !tool->is_valid()))
            {
                delete tool;
                continue;
            }

            loaded.append(tool);
        }
    }

    default_tools_.clear();

    {
        std::size_t count = data.value(QLatin1String(COOK_TOOL_DEFAULT_COUNT), 0).toUInt();
        for(std::size_t i = 0; i < count; ++i)
        {
            Core::Id type_id = Core::Id::fromSetting(data.value(QLatin1String(COOK_TOOL_DEFAULT_KEY) + QString::number(i)));
            Core::Id id = Core::Id::fromSetting(data.value(QLatin1String(COOK_TOOL_DEFAULT_VALUE) + QString::number(i)));

            default_tools_.insert(type_id, id);

        }
    }

    return loaded;
}


void Manager::save_tools_()
{
    if(!writer_)
        initialize_writer_();

    QVariantMap data;
    data.insert(QLatin1String(COOK_TOOL_FILE_VERSION_KEY), 1);

    data.insert(QLatin1String(COOK_TOOL_DEFAULT_COUNT), default_tools_.size());
    {
        std::size_t i = 0;
        for(auto it = default_tools_.begin(); it != default_tools_.end(); ++it, ++i)
        {
            data.insert(QLatin1String(COOK_TOOL_DEFAULT_KEY) + QString::number(i), it.key().toSetting());
            data.insert(QLatin1String(COOK_TOOL_DEFAULT_VALUE) + QString::number(i), it.value().toSetting());
        }
    }


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
            data.insert(QString::fromLatin1(COOK_TOOL_TYPE_ID) + QString::number(count), tool->type_id().toSetting());
            ++count;
        }
    }

    data.insert(QLatin1String(COOK_TOOL_COUNT_KEY), count);
    writer_->save(data, Core::ICore::mainWindow());
}


QList<Tool *> Manager::autodetect_tools_(const Factory & factory)
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
    QStringList execs;
    for(const QString & possible_name : factory.executable_names())
        execs.append(env.appendExeExtensions(possible_name));

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
        auto tool = factory.construct(Tool::AutoDetected, Tool::generate_id());
        tool->set_exec_file(info);
        tool->set_display_name( QString("System %1 at %2").arg(tool_name_).arg(candidate));

        // only valid tools should be in the list
        if (!tool->is_valid())
            delete tool;
        else
            auto_detected << tool;
    }

    return auto_detected;
}

const ToolFactoryInterface * Manager::factory(const Core::Id & type_id) const
{
    return factories_.value(type_id, nullptr);
}

void Manager::register_factory(const Core::Id & type_id, ToolFactoryInterface * factory)
{
    QTC_ASSERT(factories_.find(type_id) == factories_.end(), return);
    factories_[type_id] = factory;
}


} }
