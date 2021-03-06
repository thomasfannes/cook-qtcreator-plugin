#ifndef HEADER_cook_chai_editor_Indenter_hpp_ALREADY_INCLUDED
#define HEADER_cook_chai_editor_Indenter_hpp_ALREADY_INCLUDED

#include <texteditor/indenter.h>

namespace cook { namespace chai { namespace editor {

class Indenter : public TextEditor::Indenter
{
public:
    bool isElectricCharacter(const QChar &) const override { return false; }
    void indentBlock(QTextDocument*, const QTextBlock &block, const QChar &, const TextEditor::TabSettings &settings) override;
};

} } }

#endif
