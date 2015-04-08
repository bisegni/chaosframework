#-------------------------------------------------
#
# Project created by QtCreator 2015-03-31T11:15:31
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ccs
TEMPLATE = app
RESOURCES = qdarkstyle/style.qrc \
    dark_orange/dark_orange_style.qrc

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
    data/ChaosData.cpp \
    api_async_processor/ApiAsyncProcessor.cpp \
    api_async_processor/ApiAsyncRunnable.cpp \
    MainController.cpp \
    node/unit_server/UnitServerEditor.cpp \
    node/control_unit/ControUnitInstanceEditor.cpp

HEADERS  += mainwindow.h \
    search/SearchNodeResult.h \
    search/SearchNode.h \
    presenter/PresenterWidget.h \
    presenter/CommandPresenter.h \
    data/ChaosData.h \
    api_async_processor/ApiAsyncProcessor.h \
    api_async_processor/ApiAsyncRunnable.h \
    MainController.h \
    node/unit_server/UnitServerEditor.h \
    node/control_unit/ControUnitInstanceEditor.h

FORMS    += mainwindow.ui \
    search/searchnoderesult.ui \
    search/searchnode.ui \
    node/unit_server/UnitServerEditor.ui \
    node/control_unit/ControUnitInstanceEditor.ui

DISTFILES += \
    dark_orange.stylesheet \
    node/unit_server/Test.qml \
    node/unit_server/TestForm.ui.qml
