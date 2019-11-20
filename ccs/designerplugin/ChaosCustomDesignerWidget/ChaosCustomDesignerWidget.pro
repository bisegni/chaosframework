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

PRECOMPILED_HEADER = precomp_plugin_header.h
CONFIG += precomp_plugin_header
include(../../widget/designerui.pri)

INCLUDEPATH += $$PWD/../../../usr/local/include

HEADERS += CDatasetAttributeLabelMenu.h \
           CChangeSetGlobalCommitPlugin.h \
           CChangeSetGlobalRollbackPlugin.h \
           CDatasetAttributeHealthLabelPlugin.h \
           CDatasetAttributeImagePlugin.h \
           CDatasetAttributeLabelDialog.h \
           CDatasetAttributeLabelPlugin.h \
           CDatasetAttributeSet2VButtonPlugin.h \
           CDatasetAttributeSetValueComboBoxDialog.h \
           CDatasetAttributeSetValueComboBoxMenuExtension.h \
           CDatasetAttributeSetValueComboBoxPlugin.h \
           CDatasetAttributeSetValueLineEditPlugin.h \
           CUOnlineLedIndicatorPlugin.h \
           ChaosWidgets.h \
           precomp_header.h
SOURCES += CDatasetAttributeLabelMenu.cpp \
           CChangeSetGlobalCommitPlugin.cpp \
           CChangeSetGlobalRollbackPlugin.cpp \
           CDatasetAttributeHealthLabelPlugin.cpp \
           CDatasetAttributeImagePlugin.cpp \
           CDatasetAttributeLabelDialog.cpp \
           CDatasetAttributeLabelPlugin.cpp \
           CDatasetAttributeSet2VButtonPlugin.cpp \
           CDatasetAttributeSetValueComboBoxDialog.cpp \
           CDatasetAttributeSetValueComboBoxMenuExtension.cpp \
           CDatasetAttributeSetValueComboBoxPlugin.cpp \
           CDatasetAttributeSetValueLineEditPlugin.cpp \
           CUOnlineLedIndicatorPlugin.cpp \
           ChaosWidgets.cpp
OTHER_FILES += CDatasetAttributeLabelPlugin.json

RESOURCES += \
    ../../theme/theme.qrc

DISTFILES += \
    CMakeLists.txt
