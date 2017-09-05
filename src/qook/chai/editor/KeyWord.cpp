#include "qook/chai/editor/KeyWord.hpp"

#include <QMap>

namespace qook { namespace chai { namespace editor {

namespace  {

QMap<KeyWordType, KeyWordSet> construct_keyword_map()
{
    QMap<KeyWordType, KeyWordSet> result;
    result[KeyWordType::Conditional] = { "if", "else" };
    result[KeyWordType::Repeat] = { "while", "for", "do" };
    result[KeyWordType::Statement] = { "break", "continue", "return" };
    result[KeyWordType::Exception] = { "try", "catch", "throw" };
    result[KeyWordType::KeyWord] = { "def", "true", "false", "attr", "class" };
    result[KeyWordType::Type] = {"fun", "var", "auto" };
    result[KeyWordType::BuiltInFunc] = {"eval", "throw", "clone" };

    return result;
}

const QMap<KeyWordType, KeyWordSet> & keyword_map()
{
    static QMap<KeyWordType, KeyWordSet> map = construct_keyword_map();
    return map;
}

KeyWordSet construct_all_keywords()
{
    KeyWordSet result;

    for( auto it = keyword_map().begin(); it != keyword_map().end(); ++it)
        result.unite(it.value());

    return result;
}

}

const KeyWordSet & keywords(KeyWordType type)
{
    static KeyWordSet empty;
    auto it = keyword_map().find(type);

    return (it == keyword_map().end() ? empty : it.value());
}

const KeyWordSet & all_keywords()
{
    static KeyWordSet vct = construct_all_keywords();
    return vct;
}

} } }
