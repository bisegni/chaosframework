QT      += widgets designer

QTDIR_build {
# This is only for the Qt build. Do not use externally. We mean it.
PLUGIN_TYPE = designer
PLUGIN_CLASS_NAME = ChaosCustomDesignerWidget
load(qt_plugin)
CONFIG += install_ok
} else {

#! [1]
TEMPLATE = lib
CONFIG  += plugin
#! [1]

TARGET = $$qtLibraryTarget($$TARGET)

target.path = $$[QT_INSTALL_PLUGINS]/designer
INSTALLS += target

}

include(../../widget/designerui.pri)
#PRECOMPILED_HEADER = $$PWD/../../precomp_header.h
#CONFIG += $$PWD/../../precompile_header
INCLUDEPATH += $$PWD/../../../usr/local/include

HEADERS += CDatasetAttributeLabelMenu.h \
           CDatasetAttributeLabelDialog.h \
           CDatasetAttributeLabelPlugin.h \
           CUOnlineLedIndicatorPlugin.h \
           ChaosWidgets.h
SOURCES += CDatasetAttributeLabelMenu.cpp \
           CDatasetAttributeLabelDialog.cpp \
           CDatasetAttributeLabelPlugin.cpp \
           CUOnlineLedIndicatorPlugin.cpp \
           ChaosWidgets.cpp
OTHER_FILES += CDatasetAttributeLabelPlugin.json

RESOURCES += \
    ../../theme/theme.qrc
