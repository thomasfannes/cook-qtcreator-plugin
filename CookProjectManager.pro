isEmpty(IDE_SOURCE_TREE): IDE_SOURCE_TREE = "/home/tfannes/programming/CookProjectManager/qt-creator"
#isEmpty(IDE_BUILD_TREE): IDE_BUILD_TREE = "/home/tfannes/programming/CookProjectManager/build-qtcreator-debug"

isEmpty(IDE_BUILD_TREE): IDE_BUILD_TREE = "/usr"
USE_USER_DESTDIR = yes


QTC_PLUGIN_NAME = CookProjectManager
QTC_PLUGIN_DEPENDS = coreplugin texteditor projectexplorer cpptools
include($$IDE_SOURCE_TREE/src/qtcreatorplugin.pri)

CONFIG += qjson

SRC_DIR = $${PWD}
INCLUDEPATH += $${SRC_DIR}/src \
    $${SRC_DIR}/gubg.io/src \
    $${SRC_DIR}/gubg.std/src \
    $${SRC_DIR}/gubg.extra/src

SOURCES += \
    src/cook/plugin/Plugin.cpp \
    src/cook/chai/editor/Highlighter.cpp \
    src/cook/chai/editor/Scanner.cpp \
    src/cook/chai/editor/KeyWord.cpp \
    src/cook/chai/editor/Indenter.cpp \
    src/cook/chai/editor/Editor.cpp \
    src/cook/toolset/SettingsWidget.cpp \
    src/cook/toolset/ConfigItemModel.cpp \
    src/cook/toolset/ConfigItem.cpp \
    src/cook/toolset/KitInformation.cpp \
    src/cook/toolset/Tool.cpp \
    src/cook/toolset/CookTool.cpp \
    src/cook/toolset/NinjaTool.cpp \
    src/cook/toolset/Settings.cpp \
    src/cook/toolset/KitConfigWidget.cpp \
    src/cook/project/Project.cpp \
    src/cook/project/BuildConfiguration.cpp \
    src/cook/project/ProjectNodes.cpp \
    src/cook/project/BuildSettingsWidget.cpp \
    src/cook/project/BuildConfigurationFactory.cpp \
    src/cook/project/RunConfiguration.cpp \
    src/cook/project/NinjaBuildStep.cpp \
    src/cook/project/NinjaBuildStepConfigWidget.cpp \
    src/cook/project/NinjaBuildStepFactory.cpp \
    src/cook/project/RunSettingsWidget.cpp \
    src/cook/project/RunConfigurationFactory.cpp \
    src/cook/project/InfoManager.cpp \
    src/cook/project/info/Types.cpp \
    src/cook/project/CookBuildTarget.cpp \
    src/cook/plugin/Manager.cpp \
    src/cook/toolset/ToolManager.cpp \
    src/cook/project/info/CookProcess.cpp \
    src/cook/project/info/StructureParser.cpp \
    src/cook/project/info/StructureProcess.cpp \
    src/cook/project/info/StructureManager.cpp \
    src/cook/project/CookNinjaStep.cpp \
    src/cook/project/CookNinjaStepFactory.cpp \
    src/cook/project/CookNinjaStepConfigWidget.cpp

HEADERS += \
    src/cook/plugin/Global.hpp \
    src/cook/plugin/Plugin.hpp \
    src/cook/Constants.hpp \
    src/cook/chai/editor/Highlighter.hpp \
    src/cook/chai/editor/Scanner.hpp \
    src/cook/chai/editor/FormatToken.hpp \
    src/cook/chai/editor/KeyWord.hpp \
    src/cook/chai/editor/Indenter.hpp \
    src/cook/chai/editor/Editor.hpp \
    src/cook/toolset/SettingsWidget.hpp \
    src/cook/toolset/Settings.hpp \
    src/cook/toolset/ConfigItemModel.hpp \
    src/cook/toolset/ConfigItem.hpp \
    src/cook/toolset/KitInformation.hpp \
    src/cook/toolset/KitConfigWidget.hpp \
    src/cook/toolset/Tool.hpp \
    src/cook/toolset/CookTool.hpp \
    src/cook/toolset/NinjaTool.hpp \
    src/cook/project/Project.hpp \
    src/cook/project/BuildConfiguration.hpp \
    src/cook/project/ProjectNodes.hpp \
    src/cook/project/BuildSettingsWidget.hpp \
    src/cook/project/BuildConfigurationFactory.hpp \
    src/cook/project/RunConfiguration.hpp \
    src/cook/project/NinjaBuildStep.hpp \
    src/cook/project/NinjaBuildStepConfigWidget.hpp \
    src/cook/project/NinjaBuildStepFactory.hpp \
    src/cook/project/CookBuildTarget.hpp \
    src/cook/project/RunSettingsWidget.hpp \
    src/cook/project/RunConfigurationFactory.hpp \
    src/cook/project/info/Types.hpp \
    src/cook/project/InfoManager.hpp \
    src/cook/project/InfoManagerType.hpp \
    src/cook/project/ParseRequestRun.hpp \
    src/cook/plugin/Manager.hpp \
    src/cook/toolset/ToolManager.hpp \
    src/cook/project/info/CookProcess.hpp \
    src/cook/project/info/StructureParser.hpp \
    src/cook/project/info/StructureProcess.hpp \
    src/cook/project/info/StructureManager.hpp \
    src/cook/project/info/AsyncProcessManager.hpp \
    src/cook/project/info/AsyncProcessManager.hxx \
    src/cook/project/CookNinjaStep.hpp \
    src/cook/project/CookNinjaStepFactory.hpp \
    src/cook/project/CookNinjaStepConfigWidget.hpp

OTHER_FILES += \
    CookProjectManager.json.in

FORMS += \
    src/cook/toolset/SettingsWidget.ui

RESOURCES += \
    CookProjectManager.qrc
