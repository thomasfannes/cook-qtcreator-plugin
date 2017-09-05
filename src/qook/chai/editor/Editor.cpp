#include "qook/chai/editor/Editor.hpp"
#include "qook/chai/editor/Highlighter.hpp"
#include "qook/chai/editor/Indenter.hpp"
#include "qook/Constants.hpp"

#include <texteditor/textdocument.h>

namespace qook { namespace chai { namespace editor {

QString Editor::contextHelpId() const
{
    return TextEditor::BaseTextEditor::contextHelpId();
}

static TextEditor::TextDocument *createCookDocument()
{
    auto doc = new TextEditor::TextDocument;
    doc->setId(constants::QOOK_EDITOR_ID);
    doc->setMimeType(QLatin1String(constants::CHAI_MIME_TYPE));
    return doc;
}


EditorFactory::EditorFactory()
{
    setId(constants::QOOK_EDITOR_ID);
    setDisplayName(tr("Chai script editor"));
    addMimeType(constants::CHAI_MIME_TYPE);
    addMimeType(constants::QOOK_MIME_TYPE);

    setDocumentCreator(createCookDocument);

    setEditorCreator([]() { return new Editor; });
    setEditorWidgetCreator([]() { return new TextEditor::TextEditorWidget; });
    setIndenterCreator([]() { return new Indenter; });
    setSyntaxHighlighterCreator([] { return new Highlighter; });
    setCommentDefinition(Utils::CommentDefinition::HashStyle);
    setCodeFoldingSupported(false);

//    setCompletionAssistProvider(new CMakeFileCompletionAssistProvider);
//    setAutoCompleterCreator([]() { return new CMakeAutoCompleter; });

//    setEditorActionHandlers(TextEditorActionHandler::UnCommentSelection
//            | TextEditorActionHandler::JumpToFileUnderCursor
//            | TextEditorActionHandler::Format);

//    ActionContainer *contextMenu = ActionManager::createMenu(Constants::M_CONTEXT);
//    contextMenu->addAction(ActionManager::command(TextEditor::Constants::JUMP_TO_FILE_UNDER_CURSOR));
//    contextMenu->addSeparator(Context(Constants::CMAKE_EDITOR_ID));
//    contextMenu->addAction(ActionManager::command(TextEditor::Constants::UN_COMMENT_SELECTION));
}


Core::IEditor * EditorFactory::createEditor()
{
    return TextEditorFactory::createEditor();
}


} } }
