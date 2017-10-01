#ifndef HEADER_cook_chai_editor_FormatToken_hpp_ALREADY_INCLUDED
#define HEADER_cook_chai_editor_FormatToken_hpp_ALREADY_INCLUDED

namespace cook { namespace chai { namespace editor {

enum Format
{
    Format_Number = 0,
    Format_String,
    Format_Keyword,
    Format_Type,
    Format_ClassField,
    Format_Operator,
    Format_Comment,
    Format_Doxygen,
    Format_Identifier,
    Format_Whitespace,
    Format_ImportedModule,

    Format_FormatsAmount
};

class FormatToken
{
public:
    FormatToken() {}

    FormatToken(Format format, int position, int length)
        : m_format(format), m_position(position), m_length(length)
    {}

    bool isEndOfBlock() { return m_position == -1; }

    Format format() const { return m_format; }
    int begin() const { return m_position; }
    int end() const { return m_position + m_length; }
    int length() const { return m_length; }

private:
    Format m_format = Format_FormatsAmount;
    int m_position = -1;
    int m_length = -1;
};

} } }

#endif
