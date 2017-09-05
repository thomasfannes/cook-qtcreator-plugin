#ifndef HEADER_qook_chai_editor_CookEditor_hpp_ALREADY_INCLUDED
#define HEADER_qook_chai_editor_CookEditor_hpp_ALREADY_INCLUDED

#include <texteditor/texteditor.h>

namespace qook { namespace chai { namespace editor {

class Editor : public TextEditor::BaseTextEditor
{
    Q_OBJECT

public:
    QString contextHelpId() const override;
};

class EditorFactory : public TextEditor::TextEditorFactory
{
    Q_OBJECT

public:
    EditorFactory();

    Core::IEditor *createEditor() override;
};

} } }

#endif
