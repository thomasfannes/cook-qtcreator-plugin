#ifndef HEADER_qook_chai_editor_KeyWord_hpp_ALREADY_INCLUDED
#define HEADER_qook_chai_editor_KeyWord_hpp_ALREADY_INCLUDED

#include <QString>
#include <QSet>

namespace qook { namespace chai { namespace editor {

using KeyWord = QString;
using KeyWordSet = QSet<KeyWord>;

enum class KeyWordType
{
    Conditional,
    Repeat,
    Statement,
    Exception,
    KeyWord,
    Type,
    BuiltInFunc,
};


const KeyWordSet & keywords(KeyWordType type);
const KeyWordSet & all_keywords();

} } }

#endif
