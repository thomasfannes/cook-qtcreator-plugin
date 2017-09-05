#ifndef HEADER_qook_chai_editor_Highlighter_hpp_ALREADY_INCLUDED
#define HEADER_qook_chai_editor_Highlighter_hpp_ALREADY_INCLUDED

#include <texteditor/textdocumentlayout.h>
#include <texteditor/syntaxhighlighter.h>

namespace qook { namespace chai { namespace editor {

class Scanner;

class Highlighter : public TextEditor::SyntaxHighlighter
{
public:
    Highlighter();

private:
    void highlightBlock(const QString &text) override;
    int highlightLine(const QString &text, int initialState);
    void highlightImport(Scanner &scanner);
};

} } }

#endif
