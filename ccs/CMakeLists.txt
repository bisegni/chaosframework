cmake_minimum_required(VERSION 2.8.11)

project(ccs)

SET(CMAKE_SYSTEM_NAME Linux)
SET(CMAKE_SYSTEM_VERSION 1)

SET(CMAKE_CXX_STANDARD 11)
SET(CMAKE_CXX_FLAGS "-fPIC")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -include precomp_header.h -w")

MESSAGE(">>Find includes in corresponding build directories")
SET(CMAKE_INCLUDE_CURRENT_DIR ON)


# Set the output folder where your program will be created
SET(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/build)
SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
SET(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})

set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

MESSAGE(">>Instruct CMake to run moc automatically when needed.")
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

#SET(CMAKE_PREFIX_PATH /home/makogo82/baltig/chaos_framework_development_bundle/chaosframework/usr/local/lib)

INCLUDE_DIRECTORIES(/home/makogo82/baltig/chaos_framework_development_bundle/chaosframework/usr/local/include)
LINK_DIRECTORIES(/home/makogo82/baltig/chaos_framework_development_bundle/chaosframework/usr/local/lib)

#SET(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
#SET(CMAKE_EXE_LINKER_FLAGS "-static")

#FIND_PACKAGE(chaos_common REQUIRED)


SET(QT_ROOT_DIR "/home/makogo82/Qt5.7.0/5.7")
MESSAGE(">>QT ROOT DIR: " ${QT_ROOT_DIR})

SET(QT_INCLUDE_DIR ${QT_ROOT_DIR}/gcc_64/include)
SET(QT_LIB_DIR ${QT_ROOT_DIR}/gcc_64/lib)
MESSAGE(">>QT INCLUDE DIR: " ${QT_INCLUDE_DIR})
MESSAGE(">>QT LIB DIR: " ${QT_LIB_DIR})

SET(QT_QMAKE_EXECUTABLE ${QT_ROOT_DIR}/gcc_64/bin/qmake)
MESSAGE(STATUS "QT_QMAKE_EXECUTABLE: " ${QT_QMAKE_EXECUTABLE})


#Including libraries
#Here’s how you actually make use of a library. As the first step, you need to instruct the compiler to find a desired library and its components
SET(QT_LIBRARIES
    ${QT_LIB_DIR}/libQt5Core.so
    ${QT_LIB_DIR}/libQt5Widgets.so
    ${QT_LIB_DIR}/libQt5Gui.so
    ${QT_LIB_DIR}/libQt5PrintSupport.so
    ${QT_LIB_DIR}/libQt5Sql.so)

INCLUDE_DIRECTORIES(${QT_LIB_DIR})

MESSAGE(">>Find the Qt5 library")

FIND_PACKAGE(Qt5Widgets REQUIRED)
FIND_PACKAGE(Qt5Core REQUIRED)
FIND_PACKAGE(Qt5Gui REQUIRED)
FIND_PACKAGE(Qt5PrintSupport REQUIRED)


#INCLUDE_DIRECTORIES("${PROJECT_SOURCE_DIR}")
INCLUDE_DIRECTORIES (${QT_INCLUDE_DIR_QtCore})
INCLUDE_DIRECTORIES(${QT_INCLUDE_DIR})

#MESSAGE( STATUS "QT_INCLUDE_DIR:         " ${QT_INCLUDE_DIR})

INCLUDE_DIRECTORIES(${QT_INCLUDE_DIR}/QtCore)
INCLUDE_DIRECTORIES(${QT_INCLUDE_DIR}/QtWidgets)
INCLUDE_DIRECTORIES(${QT_INCLUDE_DIR}/QtGui)
INCLUDE_DIRECTORIES(${QT_INCLUDE_DIR}/QtPrintSupport)
INCLUDE_DIRECTORIES(${QT_INCLUDE_DIR}/QtSql)




MESSAGE(">>QT5_WRAP_UI")
QT5_WRAP_UI(UIS_HDRS
                search/searchnoderesult.ui
                node/unit_server/UnitServerEditor.ui
                node/control_unit/ControUnitInstanceEditor.ui
                node/control_unit/DriverDescriptionInputDialog.ui
                node/control_unit/AttributeValueRangeDialog.ui
                node/data_service/DataServiceEditor.ui
                node/data_service/CreateEditDataService.ui
                preference/PreferenceDialog.ui
                monitor/healt/HealtPresenterWidget.ui
                monitor/healt/HealtWidgetsListPresenteWidget.ui
                node/control_unit/ControlUnitEditor.ui
                node/control_unit/ControlUnitCommandTemplateEditor.ui
                node/control_unit/CommandTemplateInstanceEditor.ui
                plot/NodeAttributePlotting.ui)

MESSAGE(">>qt5_wrap_cpp")
qt5_wrap_cpp(ccs_include_moc
             monitor/handler/healt/HealtHeartbeatHandler.h
             monitor/handler/healt/HealtStatusHandler.h
             monitor/handler/system/SystemControlUnitRunScheduleDelay.h
                monitor/handler/AbstractAttributeHandler.h
                monitor/handler/AbstractTSTaggedAttributeHandler.h
                monitor/handler/MonitorBinaryAttributeHandler.h
                monitor/handler/MonitorBoolAttributeHandler.h
                monitor/handler/MonitorDoubleAttributeHandler.h
                monitor/handler/MonitorInt32AttributeHandler.h
                monitor/handler/MonitorInt64AttributeHandler.h
                monitor/handler/MonitorStringAttributeHandler.h
                monitor/handler/MonitorTSTaggedBinaryAttributeHandler.h
                monitor/handler/MonitorTSTaggedDoubleAttributeHandler.h
                monitor/handler/MonitorTSTaggedInt32AttributeHandler.h
                monitor/handler/MonitorTSTaggedInt64AttributeHandler.h
                monitor/handler/MonitorTSTaggedStringAttributeHandler.h
                monitor/handler/MonitorTSTaggetBoolAttributeHandler.h)


MESSAGE(">>Tell CMake to create the helloworld executable")

QT5_WRAP_CPP(HEADERS_MOC ${QT_HEADERS})
QT5_ADD_RESOURCES(RESOURCES_RCC ${RESOURCES})

#FILE(GLOB SRCS *.cpp)
#FILE(GLOB HEADERS *.h)



add_executable(ccs
        main.cpp
        search/SearchNodeResult.cpp
        presenter/PresenterWidget.cpp
        api_async_processor/ApiAsyncProcessor.cpp
        api_async_processor/ApiAsyncRunnable.cpp
        MainController.cpp
        node/unit_server/UnitServerEditor.cpp
        node/control_unit/ControUnitInstanceEditor.cpp
        node/control_unit/DriverDescriptionInputDialog.cpp
        node/control_unit/AttributeValueRangeDialog.cpp
        node/data_service/DataServiceEditor.cpp
        node/data_service/CreateEditDataService.cpp
        preference/PreferenceDialog.cpp
        monitor/healt/HealtMonitorWidget.cpp
        monitor/healt/HealtPresenterWidget.cpp
        monitor/healt/HealtWidgetsListPresenteWidget.cpp
        node/control_unit/ControlUnitEditor.cpp
        logic/property_switch/MultiPropertyLogicSwitch.cpp
        logic/property_switch/SwitchAggregator.cpp
        node/control_unit/ControlUnitCommandTemplateEditor.cpp
        node/control_unit/CommandTemplateInstanceEditor.cpp
        data/CommandReader.cpp
        data/CommandParameterReader.cpp
        widget/CDSAttrQLineEdit.cpp
        data/ChaosTypedAttributeValueSetter.cpp
        external_lib/qcustomplot.cpp
        data/AttributeReader.cpp
        data/ChaosByteArray.cpp
        plot/NodeAttributePlotting.cpp
        data/DatasetReader.cpp
        data/DatasetAttributeReader.cpp
        widget/CLedIndicatorHealt.cpp
        preference/PreferenceMDSHostListModel.cpp
        api_async_processor/ApiHandler.cpp
        #${SRCS}
        #${HEADERS}
        ${ccs_include_moc}
        ${UIS_HDRS}
        ${QT5_WRAP_CPP}
        ${QT5_ADD_RESOURCES}
)


MESSAGE(">>Target libraries")
target_link_libraries(ccs
                      chaos_common
                      chaos_metadata_service_client
                      zmq
                      boost_chrono
                      boost_log
                      boost_log_setup
                      boost_system
                      boost_thread
                      boost_program_options
                      boost_regex
                      boost_filesystem
                      #mongoose
                      jsoncpp)


MESSAGE(">>END")



