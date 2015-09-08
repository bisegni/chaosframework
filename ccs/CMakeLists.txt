cmake_minimum_required(VERSION 2.8.11)

project(ccs)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

INCLUDE_DIRECTORIES( ../usr/local/include )
LINK_DIRECTORIES( ../usr/local/lib )

# Find the QtWidgets library
find_package(Qt5Widgets)
find_package(Qt5Core)
find_package(Qt5Gui)
find_package(Qt5PrintSupport)

QT5_WRAP_UI(UIS_HDRS
    search/searchnoderesult.ui
    search/searchnode.ui
    node/unit_server/UnitServerEditor.ui
    node/control_unit/ControUnitInstanceEditor.ui
    node/control_unit/DriverDescriptionInputDialog.ui
    node/control_unit/AttributeValueRangeDialog.ui
    node/data_service/DataServiceEditor.ui
    node/data_service/CreateEditDataService.ui
    preference/PreferenceDialog.ui
    MainWindow.ui
    monitor/healt/HealtPresenterWidget.ui
    monitor/healt/HealtWidgetsListPresenteWidget.ui
    node/control_unit/ControlUnitEditor.ui
    node/control_unit/ControlUnitCommandTemplateEditor.ui
    node/control_unit/CommandTemplateInstanceEditor.ui
    plot/NodeAttributePlotting.ui)

qt5_wrap_cpp(ccs_include_moc monitor/handler/healt/HealtHeartbeatHandler.h
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
# Tell CMake to create the helloworld executable
add_executable(ccs main.cpp
    search/SearchNodeResult.cpp
    search/SearchNode.cpp
    presenter/PresenterWidget.cpp
    presenter/CommandPresenter.cpp
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
    node/data/FixedOutputChannelDatasetTableModel.cpp
    MainWindow.cpp
    monitor/healt/HealtMonitorWidget.cpp
    monitor/healt/HealtPresenterWidget.cpp
    monitor/healt/HealtWidgetsListPresenteWidget.cpp
    widget/LedIndicatorWidget.cpp
    node/data/ChaosAbstractTableModel.cpp
    node/control_unit/ControlUnitEditor.cpp
    node/data/FixedInputChannelDatasetTableModel.cpp
    logic/property_switch/MultiPropertyLogicSwitch.cpp
    logic/property_switch/SwitchAggregator.cpp
    node/data/ChaosAbstractDataSetTableModel.cpp
    node/data/ChaosAbstractListModel.cpp
    node/data/CommandListModel.cpp
    widget/list/delegate/TwoLineInformationListItemDelegate.cpp
    widget/list/delegate/TwoLineInformationItem.cpp
    node/data/CommandParameterTableModel.cpp
    node/data/AttributeValueChangeSet.cpp
    node/data/CommandTemplateListModel.cpp
    node/control_unit/ControlUnitCommandTemplateEditor.cpp
    node/control_unit/CommandTemplateInstanceEditor.cpp
    data/CommandReader.cpp
    data/CommandParameterReader.cpp
    widget/CDSAttrQLineEdit.cpp
    data/ChaosTypedAttributeValueSetter.cpp
    external_lib/qcustomplot.cpp
    data/AttributeReader.cpp
    widget/ChaosLabel.cpp
    widget/ChaosDatasetLabel.cpp
    data/ChaosByteArray.cpp
    plot/NodeAttributePlotting.cpp
    node/data/DatasetAttributeListModel.cpp
    data/DatasetReader.cpp
    data/DatasetAttributeReader.cpp
    widget/CLedIndicatorHealt.cpp
    preference/PreferenceMDSHostListModel.cpp
    ${ccs_include_moc}
    ${UIS_HDRS})

# Use the Widgets module from Qt 5.
target_link_libraries(ccs
Qt5::Widgets
Qt5::Core
Qt5::Gui
Qt5::PrintSupport
zmq
chaos_common
chaos_metadata_service_client
boost_chrono
boost_log
boost_log_setup
boost_system
boost_thread
boost_program_options
boost_regex
boost_filesystem
mongoose
jsoncpp
memcached)
