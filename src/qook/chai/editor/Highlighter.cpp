#include "qook/chai/editor/Highlighter.hpp"
#include "qook/chai/editor/Scanner.hpp"

#include <texteditor/textdocument.h>
#include <texteditor/texteditorconstants.h>
#include <utils/qtcassert.h>

namespace qook { namespace chai { namespace editor {

static TextEditor::TextStyle styleForFormat(int format)
{
    using namespace TextEditor;
    const auto f = Format(format);
    switch (f) {
    case Format_Number: return C_NUMBER;
    case Format_String: return C_STRING;
    case Format_Keyword: return C_KEYWORD;
    case Format_Type: return C_TYPE;
    case Format_ClassField: return C_FIELD;
    case Format_Operator: return C_OPERATOR;
    case Format_Comment: return C_COMMENT;
    case Format_Doxygen: return C_DOXYGEN_COMMENT;
    case Format_Identifier: return C_TEXT;
    case Format_Whitespace: return C_VISUAL_WHITESPACE;
    case Format_ImportedModule: return C_STRING;
    case Format_FormatsAmount:
        QTC_CHECK(false); // should never get here
        return C_TEXT;
    }
    QTC_CHECK(false); // should never get here
    return C_TEXT;
}

Highlighter::Highlighter()
{
    setTextFormatCategories(Format_FormatsAmount, styleForFormat);
}

void Highlighter::highlightBlock(const QString &text)
{
    int initialState = previousBlockState();
    if (initialState == -1)
        initialState = 0;
    setCurrentBlockState(highlightLine(text, initialState));
}

static bool isImportKeyword(const QString &keyword)
{
    return keyword == "import" || keyword == "from";
}

int Highlighter::highlightLine(const QString &text, int initialState)
{
    Scanner scanner(text.constData(), text.size());
    scanner.setState(initialState);

    FormatToken tk;
    bool hasOnlyWhitespace = true;
    while (!(tk = scanner.read()).isEndOfBlock()) {
        Format format = tk.format();
        if (format == Format_Keyword && isImportKeyword(scanner.value(tk)) && hasOnlyWhitespace) {
            setFormat(tk.begin(), tk.length(), formatForCategory(format));
            highlightImport(scanner);
        } else if (format == Format_Comment
                   || format == Format_String
                   || format == Format_Doxygen) {
            setFormatWithSpaces(text, tk.begin(), tk.length(), formatForCategory(format));
        } else {
            setFormat(tk.begin(), tk.length(), formatForCategory(format));
        }
        if (format != Format_Whitespace)
            hasOnlyWhitespace = false;
    }
    return scanner.state();
}

void Highlighter::highlightImport(Scanner &scanner)
{
    FormatToken tk;
    while (!(tk = scanner.read()).isEndOfBlock()) {
        Format format = tk.format();
        if (tk.format() == Format_Identifier)
            format = Format_ImportedModule;
        setFormat(tk.begin(), tk.length(), formatForCategory(format));
    }
}


} } }
