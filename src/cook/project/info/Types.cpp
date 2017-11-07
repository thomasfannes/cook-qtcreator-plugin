#include "cook/project/info/Types.hpp"
#include <QVariantMap>
#include <QVariant>
#include <QDebug>

namespace cook { namespace project { namespace info {

Core::Id Element::to_id() const
{
    QString string_rep;

    if (name.isNull())
        string_rep = QString("|%1").arg(uri);
    else
    {
        unsigned int size = uri.size();
        string_rep = QString("%1|%2%3").arg(size).arg(uri).arg(name);
    }

    return Core::Id::fromSetting(string_rep);
}

bool Element::from_id(const Core::Id & id)
{
    uri.clear();
    name.clear();

    QString str = id.toSetting().toString();
    int line_pos = str.indexOf('|');



    if (line_pos < 0)
    {
        return false;
    }
    else if (line_pos == 0)
    {
        uri = str.mid(1);
    }
    else
    {
        bool ok = true;
        unsigned int size = str.left(line_pos).toUInt(&ok);
        if(!ok)
            return false;

        int endpos = line_pos + 1 + size;
        if (endpos > str.size())
            return false;

        uri = str.mid(line_pos+1, size);
        name = str.mid(line_pos+1 + size);
    }

    return true;
}

QString short_display_name(const Recipe & recipe)
{
    QString val = recipe.tag;

    if (val.isEmpty())
    {
        if (!recipe.name.isEmpty())
            val = recipe.name;
        else
            val = recipe.script.parentDir().toString();
    }
    else
    {
        if (!recipe.name.isEmpty())
            val.append(QString(" (%1)").arg(recipe.name));
    }

    return val;
}

QString long_display_name(const Element & recipe)
{
    QString val = recipe.uri;

    if (val.isEmpty())
    {
        if (!recipe.name.isEmpty())
            val = recipe.name;
        else
            val = recipe.script.parentDir().toString();
    }
    else
    {
        if (!recipe.name.isEmpty())
            val.append(QString(" (%1)").arg(recipe.name));
    }

    return val;
}


} } }
