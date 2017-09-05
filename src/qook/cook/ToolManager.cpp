#include "ToolManager.hpp"

namespace qook { namespace cook {

ToolManager * ToolManager::instance_ = nullptr;

ToolManager::ToolManager(QObject *parent)
    : QObject(parent),
      default_tool_(nullptr)
{
    connect(this, &ToolManager::tool_added, this, &ToolManager::tools_changed);
    connect(this, &ToolManager::tool_removed, this, &ToolManager::tools_changed);
}

ToolManager::~ToolManager()
{
    qDeleteAll(registered_tools_);
}

ToolManager * ToolManager::create_instance(QObject * parent)
{
    if(instance_ != nullptr)
        return nullptr;

    instance_ = new ToolManager(parent);
    return instance_;
}

ToolManager * ToolManager::instance()
{
    return instance_;
}

void ToolManager::deregister_tool(const Core::Id & id) const
{
    auto find_pos = registered_tools_.end();
    {
        // find the position of tool with this id
        for(auto it = registered_tools_.begin(); it != registered_tools_.end() && find_pos == registered_tools_.end(); ++it)
            if ((*it)->id() == id)
                find_pos = it;
    }

    // did we find the tool?
    if(find_pos != registered_tools_.end())
    {
        // remove from the list
        Tool * to_remove = *it;
        registered_tools_.erase(it);

        // change the default tool ?
        if (default_tool_ == to_remove)
        {
            default_tool_ = (registered_tools_.empty() ? nullptr : registered_tools_.first());
            emit default_tool_changed();
        }

        // remove it
        emit tool_removed(id);
        delete to_remove;
    }
}

Core::Id ToolManager::register_or_find_tool(const Utils::FileName & command) const
{
    // check for existing tool?
    {
        Tool * t = find_by_id(command);
        if (t != nullptr)
            return t->id();
    }

    // create a new tool
    Tool * tool = nullptr;
    {
        tool = new Tool(Tool::generate_id());
        registered_tools_.append(tool);
    }

    // set as default if no valid default is set
    Q_ASSERT(set_default_tool(tool->id()));

    emit tool_added(tool->id());
    return tool;

}

bool ToolManager::set_default_tool(const Core::Id & id) const
{
    Tool * tool = find_by_id(id);
    if (!tool)
        return false;

    default_tool_ = tool;
    emit default_tool_changed();

    return true;
}

Tool * ToolManager::find_by_command(const Utils::FileName &command) const
{
    for(Tool * t : registered_tools_)
        if (t->filename() == command.fileName())
            return t;

    return nullptr;
}

Tool * ToolManager::find_by_id(const Core::Id &id) const
{
    for (Tool * t : registered_tools_)
        if (t->id == id)
            return t;

    return nullptr;
}

} }
