isEmpty(IDE_SOURCE_TREE): IDE_SOURCE_TREE = "/home/tfannes/programming/qt-creator"
isEmpty(IDE_BUILD_TREE): IDE_BUILD_TREE = "/home/tfannes/programming/build-qtcreator-debug"
QTC_PLUGIN_NAME = QookPlugin
QTC_PLUGIN_DEPENDS = coreplugin texteditor projectexplorer
include($$IDE_SOURCE_TREE/src/qtcreatorplugin.pri)

CONFIG += qjson

# QookPlugin files
SRC_DIR = $${PWD}
INCLUDEPATH += "$${SRC_DIR}/src"

SOURCES += \
    src/qook/QookProject.cpp \
    src/qook/plugin/Plugin.cpp \
    src/qook/ProjectWizard.cpp \
    src/qook/chai/editor/Highlighter.cpp \
    src/qook/chai/editor/Scanner.cpp \
    src/qook/chai/editor/KeyWord.cpp \
    src/qook/chai/editor/Indenter.cpp \
    src/qook/chai/editor/Editor.cpp \
    src/qook/cook/ToolSelection.cpp \
    src/qook/cook/ToolSettings.cpp \
    src/qook/cook/Tool.cpp \
    src/qook/cook/ToolManager.cpp

HEADERS += \
    src/qook/QookProject.hpp \
    src/qook/plugin/Global.hpp \
    src/qook/plugin/Plugin.hpp \
    src/qook/Constants.hpp \
    src/qook/ProjectWizard.hpp \
    src/qook/chai/editor/Highlighter.hpp \
    src/qook/chai/editor/Scanner.hpp \
    src/qook/chai/editor/FormatToken.hpp \
    src/qook/chai/editor/KeyWord.hpp \
    src/qook/chai/editor/Indenter.hpp \
    src/qook/chai/editor/Editor.hpp \
    src/qook/cook/ToolSelection.hpp \
    src/qook/cook/ToolSettings.hpp \
    src/qook/cook/Tool.hpp \
    src/qook/cook/ToolManager.hpp

OTHER_FILES += \
    QookPlugin.json.in

FORMS += \
    src/qook/cook/ToolSelection.ui
