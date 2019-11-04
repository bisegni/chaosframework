#! [0]
QT      += widgets designer
#! [0]

QTDIR_build {
# This is only for the Qt build. Do not use externally. We mean it.
PLUGIN_TYPE = designer
PLUGIN_CLASS_NAME = CDatasetAttributeLabelPlugin
load(qt_plugin)
CONFIG += install_ok
} else {
# Public example:

#! [1]
TEMPLATE = lib
CONFIG  += plugin
#! [1]

TARGET = $$qtLibraryTarget($$TARGET)

target.path = $$[QT_INSTALL_PLUGINS]/designer
INSTALLS += target

}

include(../../widget/designer/CDatasetAttributeLabel/CDatasetAttributeLabel.pri)

#! [2]
HEADERS += CDatasetAttributeLabelMenu.h \
           CDatasetAttributeLabelDialog.h \
           CDatasetAttributeLabelPlugin.h
SOURCES += CDatasetAttributeLabelMenu.cpp \
           CDatasetAttributeLabelDialog.cpp \
           CDatasetAttributeLabelPlugin.cpp
OTHER_FILES += CDatasetAttributeLabelPlugin.json
#! [2]
