#-------------------------------------------------
#
# Project created by QtCreator 2015-03-31T11:15:31
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ccs
TEMPLATE = app

INCLUDEPATH += $$PWD/../
INCLUDEPATH += $$PWD/../usr/local/include

LIBS += -L$$PWD/../usr/local/lib

LIBS +=     -lzmq\
            -lchaos_common\
            -lchaos_metadata_service_client\
            -lboost_chrono\
            -lboost_log\
            -lboost_log_setup\
            -lboost_system\
            -lboost_thread\
            -lboost_program_options\
            -lboost_regex\
            -lboost_filesystem

SOURCES += main.cpp\
        mainwindow.cpp \
    search/SearchNodeResult.cpp \
    search/SearchNode.cpp \
    presenter/PresenterWidget.cpp \
    presenter/CommandPresenter.cpp \
    async_worker/AsyncWorker.cpp \
    async_worker/ApiWorker.cpp \
    data/ChaosData.cpp

HEADERS  += mainwindow.h \
    search/SearchNodeResult.h \
    search/SearchNode.h \
    presenter/PresenterWidget.h \
    presenter/CommandPresenter.h \
    async_worker/AsyncWorker.h \
    async_worker/ApiWorker.h \
    data/ChaosData.h

FORMS    += mainwindow.ui \
    search/searchnoderesult.ui \
    search/searchnode.ui

DISTFILES +=
