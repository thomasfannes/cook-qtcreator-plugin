isEmpty(IDE_SOURCE_TREE): IDE_SOURCE_TREE = "/home/tfannes/programming/qt-creator"
isEmpty(IDE_BUILD_TREE): IDE_BUILD_TREE = "/home/tfannes/programming/build-qt-creator-debug"
QTC_PLUGIN_NAME = QookPlugin
QTC_PLUGIN_DEPENDS = coreplugin texteditor projectexplorer cpptools
include($$IDE_SOURCE_TREE/src/qtcreatorplugin.pri)

CONFIG += qjson

# QookPlugin files
SRC_DIR = $${PWD}
INCLUDEPATH += $${SRC_DIR}/src \
    $${SRC_DIR}/gubg.io/src \
    $${SRC_DIR}/gubg.std/src \
    $${SRC_DIR}/gubg.extra/src

SOURCES += \
    src/qook/plugin/Plugin.cpp \
    src/qook/chai/editor/Highlighter.cpp \
    src/qook/chai/editor/Scanner.cpp \
    src/qook/chai/editor/KeyWord.cpp \
    src/qook/chai/editor/Indenter.cpp \
    src/qook/chai/editor/Editor.cpp \
    src/qook/toolset/SettingsWidget.cpp \
    src/qook/toolset/ConfigItemModel.cpp \
    src/qook/toolset/ConfigItem.cpp \
    src/qook/toolset/KitInformation.cpp \
    src/qook/toolset/Tool.cpp \
    src/qook/toolset/CookTool.cpp \
    src/qook/toolset/NinjaTool.cpp \
    src/qook/toolset/Settings.cpp \
    src/qook/toolset/Manager.cpp \
    src/qook/toolset/KitConfigWidget.cpp \
    src/qook/project/Project.cpp \
    src/qook/project/BuildConfiguration.cpp \
    src/qook/project/ProjectNodes.cpp \
    src/qook/project/BuildSettingsWidget.cpp \
    src/qook/project/BuildConfigurationFactory.cpp \
    src/qook/project/RunConfiguration.cpp \
    src/qook/project/NinjaBuildStep.cpp \
    src/qook/project/NinjaBuildStepConfigWidget.cpp \
    src/qook/project/NinjaBuildStepFactory.cpp \
    src/qook/project/RunSettingsWidget.cpp \
    src/qook/project/RunConfigurationFactory.cpp \
    src/qook/project/info/RecipesParser.cpp \
    src/qook/project/info/RecipesManager.cpp \
    src/qook/project/InfoManager.cpp \
    src/qook/project/info/BuildRecipesManager.cpp \
    src/qook/project/info/BuildRecipesParser.cpp \
    src/qook/project/info/Types.cpp \
    src/qook/project/CookBuildTarget.cpp \
    src/qook/project/BuildNinjaManager.cpp



HEADERS += \
    src/qook/plugin/Global.hpp \
    src/qook/plugin/Plugin.hpp \
    src/qook/Constants.hpp \
    src/qook/chai/editor/Highlighter.hpp \
    src/qook/chai/editor/Scanner.hpp \
    src/qook/chai/editor/FormatToken.hpp \
    src/qook/chai/editor/KeyWord.hpp \
    src/qook/chai/editor/Indenter.hpp \
    src/qook/chai/editor/Editor.hpp \
    src/qook/toolset/SettingsWidget.hpp \
    src/qook/toolset/Settings.hpp \
    src/qook/toolset/ConfigItemModel.hpp \
    src/qook/toolset/ConfigItem.hpp \
    src/qook/toolset/KitInformation.hpp \
    src/qook/toolset/KitConfigWidget.hpp \
    src/qook/toolset/Tool.hpp \
    src/qook/toolset/CookTool.hpp \
    src/qook/toolset/NinjaTool.hpp \
    src/qook/toolset/Manager.hpp \
    src/qook/project/Project.hpp \
    src/qook/project/BuildConfiguration.hpp \
    src/qook/project/ProjectNodes.hpp \
    src/qook/project/BuildSettingsWidget.hpp \
    src/qook/project/BuildConfigurationFactory.hpp \
    src/qook/project/RunConfiguration.hpp \
    src/qook/project/NinjaBuildStep.hpp \
    src/qook/project/NinjaBuildStepConfigWidget.hpp \
    src/qook/project/NinjaBuildStepFactory.hpp \
    src/qook/project/CookBuildTarget.hpp \
    src/qook/project/RunSettingsWidget.hpp \
    src/qook/project/RunConfigurationFactory.hpp \
    src/qook/project/info/RecipesParser.hpp \
    src/qook/project/info/RecipesManager.hpp \
    src/qook/project/info/AsyncManager.hpp \
    src/qook/project/info/AsyncManager.hxx \
    src/qook/project/info/Types.hpp \
    src/qook/project/info/Parser.hpp \
    src/qook/project/info/Parser.hxx \
    src/qook/project/InfoManager.hpp \
    src/qook/project/InfoManagerType.hpp \
    src/qook/project/ParseRequestRun.hpp \
    src/qook/project/info/BuildRecipesManager.hpp \
    src/qook/project/info/BuildRecipesParser.hpp \
    src/qook/project/BuildNinjaManager.hpp





OTHER_FILES += \
    QookPlugin.json.in

FORMS += \
    src/qook/toolset/SettingsWidget.ui

RESOURCES += \
    qook.qrc
