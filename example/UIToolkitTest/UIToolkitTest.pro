#-------------------------------------------------
#
# Project created by QtCreator 2012-01-07T11:56:52
#
#-------------------------------------------------

QT       += core gui

TARGET = testui
TEMPLATE = app

INCLUDEPATH += /usr/local/qwt-6.0.2-svn/lib/qwt.framework/Versions/6/Headers/
INCLUDEPATH += /usr/local/qwt-6.0.2-svn/lib
LIBS += -L/usr/lib -L/usr/local/lib -L/usr/local/qwt-6.0.2-svn/lib -lqwt -lchaos_common -lchaos_uitoolkit -lboost_chrono -lmemcached -lboost_log -lboost_log_setup -lboost_system -lboost_thread -lboost_program_options -lboost_regex -lboost_filesystem -Levent -lmpio -lmsgpack -lmsgpack-rpc


SOURCES += main.cpp\
        mainwindow.cpp \
    graphwidget.cpp \
    controldialog.cpp

HEADERS  += mainwindow.h \
    graphwidget.h \
    controldialog.h

FORMS    += mainwindow.ui \
    controldialog.ui
