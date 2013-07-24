#-------------------------------------------------
#
# Project created by QtCreator 2012-01-07T11:56:52
#
#-------------------------------------------------

QT += core gui svg
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = testui
TEMPLATE = app


INCLUDEPATH += $$PWD/../../
INCLUDEPATH += $$PWD/../../usr/local/include
LIBS += -L$$PWD/../../DerivedData/CHAOSWorkspace/Build/Products/Debug
LIBS += -L$$PWD/../../usr/local/lib

macx:INCLUDEPATH += /usr/local/qwt/lib/qwt.framework/Headers
macx:LIBS += -F/usr/local/qwt/lib/ -framework qwt

unix:!macx::INCLUDEPATH += /usr/local/qwt/include
unix:!macx:LIBS += -L/usr/local/qwt/lib -lqwt

LIBS +=  -lzmq -lchaos_common -lchaos_uitoolkit -lboost_chrono -lmemcached -lboost_log -lboost_log_setup -lboost_system -lboost_thread -lboost_program_options -lboost_regex -lboost_filesystem -levent -lmpio -lmsgpack -lmsgpack-rpc


SOURCES += main.cpp\
        mainwindow.cpp \
    graphwidget.cpp \
    controldialog.cpp

HEADERS  += mainwindow.h \
    graphwidget.h \
    controldialog.h

FORMS    += mainwindow.ui \
    controldialog.ui
