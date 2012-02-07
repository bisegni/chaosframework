#-------------------------------------------------
#
# Project created by QtCreator 2012-01-07T11:56:52
#
#-------------------------------------------------

QT       += core gui

TARGET = testui
TEMPLATE = app

INCLUDEPATH += /usr/local/qwt-6.0.2-svn/lib/qwt.framework/Versions/6/Headers/
LIBS += -L/usr/local/qwt-6.0.2-svn/lib -lqwt

SOURCES += main.cpp\
        mainwindow.cpp \
    graphwidget.cpp

HEADERS  += mainwindow.h \
    graphwidget.h

FORMS    += mainwindow.ui
