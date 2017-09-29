#include "qook/toolset/ConfigItem.hpp"
#include "qook/toolset/ConfigItemModel.hpp"
#include <QVariant>
#include <QDebug>
#include <QFont>
#include <QBrush>

namespace qook { namespace toolset {


ConfigItem::ConfigItem(const ToolFactoryInterface * factory)
    : tool_(factory->construct()),
      original_tool_(nullptr),
      factory_(factory)
{
}

ConfigItem::ConfigItem(const ToolFactoryInterface * factory, const Tool * tool)
    : tool_(tool ? factory->construct(*tool) : factory->construct()),
      original_tool_(tool),
      factory_(factory)
{

}

ConfigItem::~ConfigItem()
{
    delete tool_;
}

QVariant ConfigItem::data(int column, int role) const
{
    switch(role)
    {
    case Qt::DisplayRole:
        switch (column)
        {
        // the name column
        case 0:
            return is_default() ? QString("%1 (default)").arg(tool().display_name()) : tool().display_name();

        // the location column
        case 1:
            return tool().user_file_name();

        // the version column
        case 2:
            return tool().version_string();

        default:
            return QVariant();
        }

    case Qt::FontRole:
    {
        QFont font;
        font.setBold(has_changed());
        font.setItalic(is_default());
        return font;
    }

    case Qt::ForegroundRole:
    {
        if (!tool().is_valid())
            return QBrush(Qt::red);
        else
            return TreeItem::data(column, role);
    }

    default:
        return QVariant();
    }
}

ConfigItemModel * ConfigItem::model() const
{
    return static_cast<ConfigItemModel *>(Utils::TreeItem::model());
}

bool ConfigItem::is_default() const
{
    return tool().id() == model()->default_item_id();
}

bool ConfigItem::has_changed() const
{
    return original_tool_ == 0
                    || tool().display_name() != original_tool_->display_name()
                    || tool().exec_file() != original_tool_->exec_file()
                    || is_default() != original_tool_->is_default();
}

} }
