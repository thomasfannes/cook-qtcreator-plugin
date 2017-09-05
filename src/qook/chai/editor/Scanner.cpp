#include "qook/chai/editor/Scanner.hpp"
#include "qook/chai/editor/KeyWord.hpp"

#include <QSet>

namespace qook { namespace chai { namespace editor {

namespace {

inline static bool isHexDigit(QChar ch)
{
    return ch.isDigit()
            || (ch >= 'a' && ch <= 'f')
            || (ch >= 'A' && ch <= 'F');
}

inline static bool isOctalDigit(QChar ch)
{
    return ch.isDigit() && ch != '8' && ch != '9';
}

inline static bool isBinaryDigit(QChar ch)
{
    return ch == '0' || ch == '1';
}

inline static bool isValidIntegerSuffix(QChar ch)
{
    return ch == 'l' || ch == 'L';
}

}

Scanner::Scanner(const QChar * text, const int length)
    : m_text(text), m_textLength(length), m_state(State::State_Default)
{
}

void Scanner::setState(int state)
{
    m_state = state;
}

int Scanner::state() const
{
    return m_state;
}

FormatToken Scanner::read()
{
    setAnchor();
    if (isEnd())
        return FormatToken();

    State state;
    QChar saved;

    parseState(state, saved);

    switch(state)
    {
    case State::State_String:
        return readStringLiteral(saved);
    case State::State_MultiLineString:
        return readMultiLineStringLiteral(saved);
    default:
        return onDefaultState();
    }
}

QString Scanner::value(const FormatToken & token) const
{
    return QString(m_text + token.begin(), token.length());
}

FormatToken Scanner::onDefaultState()
{
    QChar first = peek();
    move();

    if (first == '\\' && peek() == '\n') {
        move();
        return FormatToken(Format_Whitespace, anchor(), 2);
    }

    if (first == '.' && peek().isDigit())
        return readFloatNumber();

    if (first == '\'' || first == '\"')
        return readStringLiteral(first);

    if (first.isLetter() || first == '_')
        return readIdentifier();

    if (first.isDigit())
        return readNumber();

    if (first == '#') {
        if (peek() == '#')
            return readDoxygenComment();
        return readComment();
    }

    if (first.isSpace())
        return readWhiteSpace();

    return readOperator();
}

void Scanner::checkEscapeSequence(QChar quoteChar)
{
    if (peek() == '\\') {
        move();
        QChar ch = peek();
        if (ch == '\n' || ch.isNull())
            saveState(State::State_String, quoteChar);
    }
}

FormatToken Scanner::readStringLiteral(QChar quoteChar)
{
    QChar ch = peek();
    if (ch == quoteChar && peek(1) == quoteChar) {
        saveState(State::State_MultiLineString, quoteChar);
        return readMultiLineStringLiteral(quoteChar);
    }

    while (ch != quoteChar && !ch.isNull()) {
        checkEscapeSequence(quoteChar);
        move();
        ch = peek();
    }
    if (ch == quoteChar)
        clearState();
    move();
    return FormatToken(Format_String, anchor(), length());
}

FormatToken Scanner::readMultiLineStringLiteral(QChar quoteChar)
{
    for (;;) {
        QChar ch = peek();
        if (ch.isNull())
            break;
        if (ch == quoteChar && peek(1) == quoteChar && peek(2) == quoteChar) {
            clearState();
            move();
            move();
            move();
            break;
        }
        move();
    }

    return FormatToken(Format_String, anchor(), length());
}

FormatToken Scanner::readIdentifier()
{
    const KeyWordSet & keywords = all_keywords();

    QChar ch = peek();
    while (ch.isLetterOrNumber() || ch == '_') {
        move();
        ch = peek();
    }

    const QString v = QString(m_text + m_markedPosition, length());
    Format tkFormat = Format_Identifier;
    if (v == "self")
        tkFormat = Format_ClassField;
    else if (keywords.contains(v))
        tkFormat = Format_Keyword;

    return FormatToken(tkFormat, anchor(), length());
}



FormatToken Scanner::readNumber()
{
    if (!isEnd()) {
        QChar ch = peek();
        if (ch.toLower() == 'b') {
            move();
            while (isBinaryDigit(peek()))
                move();
        } else if (ch.toLower() == 'o') {
            move();
            while (isOctalDigit(peek()))
                move();
        } else if (ch.toLower() == 'x') {
            move();
            while (isHexDigit(peek()))
                move();
        } else { // either integer or float number
            return readFloatNumber();
        }
        if (isValidIntegerSuffix(peek()))
            move();
    }
    return FormatToken(Format_Number, anchor(), length());
}

FormatToken Scanner::readFloatNumber()
{
    enum
    {
        State_INTEGER,
        State_FRACTION,
        State_EXPONENT
    } state;
    state = (peek(-1) == '.') ? State_FRACTION : State_INTEGER;

    for (;;) {
        QChar ch = peek();
        if (ch.isNull())
            break;

        if (state == State_INTEGER) {
            if (ch == '.')
                state = State_FRACTION;
            else if (!ch.isDigit())
                break;
        } else if (state == State_FRACTION) {
            if (ch == 'e' || ch == 'E') {
                QChar next = peek(1);
                QChar next2 = peek(2);
                bool isExp = next.isDigit()
                        || ((next == '-' || next == '+') && next2.isDigit());
                if (isExp) {
                    move();
                    state = State_EXPONENT;
                } else {
                    break;
                }
            } else if (!ch.isDigit()) {
                break;
            }
        } else if (!ch.isDigit()) {
            break;
        }
        move();
    }

    QChar ch = peek();
    if ((state == State_INTEGER && (ch == 'l' || ch == 'L'))
            || (ch == 'j' || ch =='J'))
        move();

    return FormatToken(Format_Number, anchor(), length());
}

/**
  reads single-line python comment, started with "#"
  */
FormatToken Scanner::readComment()
{
    QChar ch = peek();
    while (ch != '\n' && !ch.isNull()) {
        move();
        ch = peek();
    }
    return FormatToken(Format_Comment, anchor(), length());
}

/**
  reads single-line python doxygen comment, started with "##"
  */
FormatToken Scanner::readDoxygenComment()
{
    QChar ch = peek();
    while (ch != '\n' && !ch.isNull()) {
        move();
        ch = peek();
    }
    return FormatToken(Format_Doxygen, anchor(), length());
}

/**
  reads whitespace
  */
FormatToken Scanner::readWhiteSpace()
{
    while (peek().isSpace())
        move();
    return FormatToken(Format_Whitespace, anchor(), length());
}

/**
  reads punctuation symbols, excluding some special
  */
FormatToken Scanner::readOperator()
{
    static const QString EXCLUDED_CHARS = "\'\"_#";
    QChar ch = peek();
    while (ch.isPunct() && !EXCLUDED_CHARS.contains(ch)) {
        move();
        ch = peek();
    }
    return FormatToken(Format_Operator, anchor(), length());
}

void Scanner::clearState()
{
    m_state = 0;
}

void Scanner::saveState(State state, QChar savedData)
{
    m_state = (state << 16) | static_cast<int>(savedData.unicode());
}

void Scanner::parseState(State &state, QChar &savedData) const
{
    state = static_cast<State>(m_state >> 16);
    savedData = static_cast<ushort>(m_state);
}

} } }
