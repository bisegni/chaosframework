cmake_minimum_required(VERSION 2.8.11)

project(ccs)

MESSAGE(">>Find the Qt5 library")
FIND_PACKAGE(Qt5Widgets REQUIRED)
FIND_PACKAGE(Qt5Core REQUIRED)
FIND_PACKAGE(Qt5Sql REQUIRED)
FIND_PACKAGE(Qt5Gui REQUIRED)
FIND_PACKAGE(Qt5PrintSupport REQUIRED)

# set(QT_USE_QTSQL 1)
SET(CMAKE_CXX_STANDARD 11)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -include precomp_header.h -w")

MESSAGE(">>Find includes in corresponding build directories")
SET(CMAKE_INCLUDE_CURRENT_DIR ON)

# Set the output folder where your program will be created
SET(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/build)
SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
SET(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})
SET(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

MESSAGE(">>Instruct CMake to run moc automatically when needed.")
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

MESSAGE(">>QT Version: " ${Qt5Widgets_VERSION})
MESSAGE(">>QT WIDGET DIR: " ${Qt5Widgets_INCLUDE_DIRS})
MESSAGE(">>QT GUI DIR: " ${Qt5Gui_INCLUDE_DIRS})
MESSAGE(">>QT SQL DIR: " ${Qt5Sql_INCLUDE_DIRS})
MESSAGE(">>QT SQL DIR: " ${Qt5PrintSupport_INCLUDE_DIRS})
MESSAGE(">>QT LIB DIR: " ${Qt5Widgets_LIBRARIES})

SET(QT_QMAKE_EXECUTABLE ${QT_ROOT_DIR}/gcc_64/bin/qmake)
MESSAGE(STATUS "QT_QMAKE_EXECUTABLE: " ${QT_QMAKE_EXECUTABLE})

#INCLUDE_DIRECTORIES(${QT_LIB_DIR})
INCLUDE_DIRECTORIES(${Qt5Widgets_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${Qt5Sql_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${Qt5PrintSupport_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${QT_INCLUDE_DIR_QtCore})
INCLUDE_DIRECTORIES(${QT_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_LIST_DIR}/../usr/local/include)
LINK_DIRECTORIES(${CMAKE_CURRENT_LIST_DIR}/../usr/local/lib)

#INCLUDE_DIRECTORIES("${PROJECT_SOURCE_DIR}")


#MESSAGE( STATUS "QT_INCLUDE_DIR:         " ${QT_INCLUDE_DIR})

# INCLUDE_DIRECTORIES(${QT_INCLUDE_DIR}/QtCore)
# INCLUDE_DIRECTORIES(${QT_INCLUDE_DIR}/QtWidgets)
# INCLUDE_DIRECTORIES(${QT_INCLUDE_DIR}/QtGui)
# INCLUDE_DIRECTORIES(${QT_INCLUDE_DIR}/QtPrintSupport)
# INCLUDE_DIRECTORIES(${QT_INCLUDE_DIR}/QtSql)

# MESSAGE(">>qt5_wrap_cpp")
# qt5_wrap_cpp(ccs_include_moc
#              monitor/handler/healt/HealtHeartbeatHandler.h
#              monitor/handler/healt/HealtStatusHandler.h
#              monitor/handler/system/SystemControlUnitRunScheduleDelay.h
#              monitor/handler/AbstractAttributeHandler.h
#               monitor/handler/AbstractTSTaggedAttributeHandler.h
#               monitor/handler/MonitorBinaryAttributeHandler.h
#               monitor/handler/MonitorBoolAttributeHandler.h
#               monitor/handler/MonitorDoubleAttributeHandler.h
#               monitor/handler/MonitorInt32AttributeHandler.h
#               monitor/handler/MonitorInt64AttributeHandler.h
#               monitor/handler/MonitorStringAttributeHandler.h
#               monitor/handler/MonitorTSTaggedBinaryAttributeHandler.h
#               monitor/handler/MonitorTSTaggedDoubleAttributeHandler.h
#               monitor/handler/MonitorTSTaggedInt32AttributeHandler.h
#               monitor/handler/MonitorTSTaggedInt64AttributeHandler.h
#               monitor/handler/MonitorTSTaggedStringAttributeHandler.h
#               monitor/handler/MonitorTSTaggetBoolAttributeHandler.h)


MESSAGE(">>Tell CMake to create the helloworld executable")

QT5_WRAP_CPP(HEADERS_MOC ${QT_HEADERS})
QT5_ADD_RESOURCES(RESOURCES_RCC ${RESOURCES})

set(sources
    main.cpp
    MainController.cpp
    GlobalServices.cpp
    api_async_processor/ApiAsyncProcessor.cpp
    api_async_processor/ApiAsyncRunnable.cpp
    api_async_processor/ApiHandler.cpp
    api_async_processor/ApiSubmitter.cpp
    data/AgentNodeAssociatedListModel.cpp
    data/ApplicationErrorLoggingTableModel.cpp
    data/AttributeReader.cpp
    data/AttributeValueChangeSet.cpp
    data/ChaosAbstractDataSetTableModel.cpp
    data/ChaosAbstractListModel.cpp
    data/ChaosAbstractTableModel.cpp
    data/ChaosByteArray.cpp
    data/ChaosTypedAttributeValueSetter.cpp
    data/CommandListModel.cpp
    data/CommandParameterReader.cpp
    data/CommandParameterTableModel.cpp
    data/CommandReader.cpp
    data/CommandTemplateListModel.cpp
    data/ControlUnitStateVaribleListModel.cpp
    data/DatasetAttributeListModel.cpp
    data/DatasetAttributeReader.cpp
    data/DatasetReader.cpp
    data/EditableDatasetTableModel.cpp
    data/EditableScriptVariableTableModel.cpp
    data/FixedInputChannelDatasetTableModel.cpp
    data/FixedOutputChannelDatasetTableModel.cpp
    data/LiveDatasetTableModel.cpp
    data/LogDataTableModel.cpp
    data/LogDomainListModel.cpp
    data/LogEntryAdvancedSearchTableModel.cpp
    data/LogEntryTableModel.cpp
    data/LogProcessEntriesTableModel.cpp
    data/ScriptInstanceListModel.cpp
    data/ScriptListModel.cpp
    data/SearchNodeListModel.cpp
    data/SnapshotDatasetNodeListModel.cpp
    data/delegate/ComboBoxDelegate.cpp
    data/delegate/TwoLineInformationItem.cpp
    data/delegate/TwoLineInformationListItemDelegate.cpp
    data/editor/common/EditableSubtypeListTableModel.cpp
    data/editor/common/EditableSubtypeListWidget.cpp
    data/editor/common/EditableSubtypeListWidget.ui
    data/editor/dataset/EditableDatasetTableModelEditDialog.cpp
    data/editor/dataset/EditableDatasetTableModelEditDialog.ui
    data/editor/script/EditableScriptVariableTableModelEditDialog.cpp
    data/editor/script/EditableScriptVariableTableModelEditDialog.ui
    data/json/JsonTableModel.cpp
    error/ErrorManager.cpp
    external_lib/qcustomplot.cpp
    external_lib/QtNodeEditor/Connection.cpp
    external_lib/QtNodeEditor/ConnectionBlurEffect.cpp
    external_lib/QtNodeEditor/ConnectionGeometry.cpp
    external_lib/QtNodeEditor/ConnectionGraphicsObject.cpp
    external_lib/QtNodeEditor/ConnectionPainter.cpp
    external_lib/QtNodeEditor/ConnectionState.cpp
    external_lib/QtNodeEditor/ConnectionStyle.cpp
    external_lib/QtNodeEditor/DataModelRegistry.cpp
    external_lib/QtNodeEditor/FlowItemInterface.cpp
    external_lib/QtNodeEditor/FlowScene.cpp
    external_lib/QtNodeEditor/FlowView.cpp
    external_lib/QtNodeEditor/FlowViewStyle.cpp
    external_lib/QtNodeEditor/Node.cpp
    external_lib/QtNodeEditor/NodeConnectionInteraction.cpp
    external_lib/QtNodeEditor/NodeDataModel.cpp
    external_lib/QtNodeEditor/NodeGeometry.cpp
    external_lib/QtNodeEditor/NodeGraphicsObject.cpp
    external_lib/QtNodeEditor/NodePainter.cpp
    external_lib/QtNodeEditor/NodeState.cpp
    external_lib/QtNodeEditor/NodeStyle.cpp
    external_lib/QtNodeEditor/Properties.cpp
    external_lib/QtNodeEditor/StyleCollection.cpp
    language_editor/CLINGLanguageSupport.cpp
    language_editor/JsonEditor.cpp
    # language_editor/JsonEditor.ui
    language_editor/LuaLanguageSupport.cpp
    language_editor/ScriptEditor.cpp
    # language_editor/ScriptEditor.ui
    log_browser/ApplicationErrorLogging.cpp
    # log_browser/ApplicationErrorLogging.ui
    log_browser/LogBrowser.cpp
    # log_browser/LogBrowser.ui
    logic/property_switch/MultiPropertyLogicSwitch.cpp
    logic/property_switch/SwitchAggregator.cpp
    node/agent/AgentEditor.cpp
    # node/agent/AgentEditor.ui
    node/agent/AgentNodeAssociationEditor.cpp
    # node/agent/AgentNodeAssociationEditor.ui
    node/agent/AgentSetting.cpp
    # node/agent/AgentSetting.ui
    node/connection_manager/ConnectionEditor.cpp
    # node/connection_manager/ConnectionEditor.ui
    node/connection_manager/model/ControlUnitNodeDataModel.cpp
    node/control_unit/AttributeValueRangeDialog.cpp
    # node/control_unit/AttributeValueRangeDialog.ui
    node/control_unit/CommandTemplateInstanceEditor.cpp
    # node/control_unit/CommandTemplateInstanceEditor.ui
    node/control_unit/ControlUnitCommandTemplateEditor.cpp
    # node/control_unit/ControlUnitCommandTemplateEditor.ui
    node/control_unit/ControlUnitEditor.cpp
    # node/control_unit/ControlUnitEditor.ui
    node/control_unit/ControUnitInstanceEditor.cpp
    # node/control_unit/ControUnitInstanceEditor.ui
    node/control_unit/DriverDescriptionInputDialog.cpp
    # node/control_unit/DriverDescriptionInputDialog.ui
    node/data_service/CreateEditDataService.cpp
    # node/data_service/CreateEditDataService.ui
    node/data_service/DataServiceEditor.cpp
    # node/data_service/DataServiceEditor.ui
    node/unit_server/UnitServerEditor.cpp
    # node/unit_serverUnitServerEditor.ui
    plot/AbstractBinaryPlotAdapter.cpp
    plot/BufferPlot.cpp
    # plot/BufferPlot.ui
    plot/NodeAttributePlotting.cpp
    # plot/NodeAttributePlotting.ui
    preference/PreferenceDialog.cpp
    # preference/PreferenceDialog.ui
    preference/PreferenceManager.cpp
    preference/PreferenceMDSHostListModel.cpp
    preference/SelectNetworkDomain.cpp
    # preference/SelectNetworkDomain.ui
    presenter/PresenterWidget.cpp
    script/CreateNewScriptDialog.cpp
    # script/CreateNewScriptDialog.ui
    script/ScriptDescriptionWidget.cpp
    # script/ScriptDescriptionWidget.ui
    script/ScriptInstanceManagerWidget.cpp
    # script/ScriptInstanceManagerWidget.ui
    script/ScriptManager.cpp
    # script/ScriptManager.ui
    script/editor/CodeEditor.cpp
    search/SearchNodeResult.cpp
    # search/searchnoderesult.ui
    snapshot/NewSnapshot.cpp
    # snapshot/NewSnapshot.ui
    snapshot/NodeInSnapshotTableModel.cpp
    snapshot/NodeListModel.cpp
    snapshot/SnapshotManager.cpp
    # snapshot/SnapshotManager.ui
    snapshot/SnapshotTableModel.cpp
    status_manager/StatusManager.cpp
    tree_group/AddNewDomain.cpp
    # tree_group/AddNewDomain.ui
    tree_group/DomainListModel.cpp
    tree_group/GroupTreeItem.cpp
    tree_group/GroupTreeModel.cpp
    tree_group/TreeGroupManager.cpp
    # tree_group/TreeGroupManager.ui
    monitor/healt/HealtMonitorWidget.cpp
    monitor/healt/HealtPresenterWidget.cpp
    monitor/healt/HealtWidgetsListPresenteWidget.cpp
    monitor/healt/HighlighterForChaosType.cpp
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
    monitor/handler/MonitorTSTaggetBoolAttributeHandler.h
    utility/MessageUtility.cpp
    utility/WidgetUtility.cpp
    widget/CCommandStatisticWidget.cpp
    widget/CControlUnitDatasetLabel.cpp
    widget/CDSAttrQLineEdit.cpp
    widget/ChaosMonitorWidgetCompanion.cpp
    widget/ChaosStorageTypeWidget.cpp
    widget/ChaosWidgetCompanion.cpp
    widget/CLedIndicatorHealt.cpp
    widget/CNodeHealthLabel.cpp
    widget/CNodeLogWidget.cpp
    widget/CNodeResourceWidget.cpp
    widget/CPopupWidgetContainer.cpp
    widget/CPropertyPushButton.cpp
    widget/CPropertyTextEdit.cpp
    widget/CQListView.cpp
    widget/CQTableView.cpp
    widget/CStateVisiblePushButton.cpp
    widget/OnOffImageIndicator.cpp
    widget/StateImageIndicatorWidget.cpp
    widget/WidgetDynamicList.cpp
    widget/StorageBurst.cpp
    widget/CLedIndicatorCUBusy.cpp
    resources.qrc
    ${ccs_include_moc}
)

add_executable(ccs
        ${sources}
        #${SRCS}
        #${HEADERS}
        ${ccs_include_moc}
        ${UIS_HDRS}

)


MESSAGE(">>Target libraries")
target_link_libraries(ccs
                      Qt5::Core
                      Qt5::Gui
                      Qt5::Sql
                      Qt5::Widgets
                      Qt5::PrintSupport
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
