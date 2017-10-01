#ifndef HEADER_cook_chai_editor_Scanner_hpp_ALREADY_INCLUDED
#define HEADER_cook_chai_editor_Scanner_hpp_ALREADY_INCLUDED

#include "cook/chai/editor/FormatToken.hpp"
#include <qchar.h>

namespace cook { namespace chai { namespace editor {

class Scanner
{
    Scanner(const Scanner &) = delete;
    Scanner & operator=(const Scanner &) = delete;

public:
    Scanner(const QChar * text, const int length);

    enum State
    {
        State_Default,
        State_String,
        State_MultiLineString
    };

    void setState(int state);
    int state() const;
    FormatToken read();
    QString value(const FormatToken & token) const;
private:
    FormatToken onDefaultState();

    void checkEscapeSequence(QChar quoteChar);
    FormatToken readStringLiteral(QChar quoteChar);
    FormatToken readMultiLineStringLiteral(QChar quoteChar);
    FormatToken readIdentifier();
    FormatToken readNumber();
    FormatToken readFloatNumber();
    FormatToken readComment();
    FormatToken readDoxygenComment();
    FormatToken readWhiteSpace();
    FormatToken readOperator();

    void clearState();
    void saveState(State state, QChar savedData);
    void parseState(State &state, QChar &savedData) const;

    void setAnchor() { m_markedPosition = m_position; }
    void move() { ++m_position; }
    int length() const { return m_position - m_markedPosition; }
    int anchor() const { return m_markedPosition; }
    bool isEnd() const { return m_position >= m_textLength; }

    QChar peek(int offset = 0) const
    {
        int pos = m_position + offset;
        if (pos >= m_textLength)
            return QLatin1Char('\0');
        return m_text[pos];
    }

    const QChar *m_text;
    const int m_textLength;
    int m_position = 0;
    int m_markedPosition = 0;

    int m_state;
};

} } }

#endif
