#-------------------------------------------------
#
# Project created by QtCreator 2015-03-31T11:15:31
#
#-------------------------------------------------

QT += core gui sql
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

PRECOMPILED_HEADER = precomp_header.h
CONFIG += precompile_header

QMAKE_CXXFLAGS += -std=c++11
#QMAKE_MAC_SDK = macosx10.12

TARGET = ccs
TEMPLATE = app
RESOURCES = theme/theme.qrc \
    resources.qrc

#INCLUDEPATH += $$PWD/../
INCLUDEPATH += $$PWD/../usr/local/include

LIBS +=     -lchaos_metadata_service_client
LIBS += -L$$PWD/../usr/local/lib
unix:!macx {
    LIBS += -L"/usr/lib/x86_64-linux-gnu/mesa/"
    LIBS += -Wl,--whole-archive -lchaos_common -Wl,--no-whole-archive
}

macx:{
    LIBS +=  -lchaos_common
}

LIBS +=     -ljsoncpp\
            -lzmq\
            -lpthread\
            -lboost_system\
            -lboost_thread\
            -lboost_program_options\
            -lboost_regex\
            -lboost_chrono\
            -lboost_filesystem\
            -lboost_log\
            -lboost_log_setup

SOURCES += main.cpp\
    search/SearchNodeResult.cpp \
    presenter/PresenterWidget.cpp \
    api_async_processor/ApiAsyncProcessor.cpp \
    api_async_processor/ApiAsyncRunnable.cpp \
    MainController.cpp \
    node/unit_server/UnitServerEditor.cpp \
    node/control_unit/ControUnitInstanceEditor.cpp \
    node/control_unit/DriverDescriptionInputDialog.cpp \
    node/control_unit/AttributeValueRangeDialog.cpp \
    node/data_service/DataServiceEditor.cpp \
    node/data_service/CreateEditDataService.cpp \
    preference/PreferenceDialog.cpp \
    monitor/healt/HealtMonitorWidget.cpp \
    monitor/healt/HealtPresenterWidget.cpp \
    monitor/healt/HealtWidgetsListPresenteWidget.cpp \
    node/control_unit/ControlUnitEditor.cpp \
    logic/property_switch/MultiPropertyLogicSwitch.cpp \
    logic/property_switch/SwitchAggregator.cpp \
    node/control_unit/ControlUnitCommandTemplateEditor.cpp \
    node/control_unit/CommandTemplateInstanceEditor.cpp \
    data/CommandReader.cpp \
    data/CommandParameterReader.cpp \
    widget/CDSAttrQLineEdit.cpp \
    data/ChaosTypedAttributeValueSetter.cpp \
    external_lib/qcustomplot.cpp \
    data/AttributeReader.cpp \
    data/ChaosByteArray.cpp \
    plot/NodeAttributePlotting.cpp \
    data/DatasetReader.cpp \
    data/DatasetAttributeReader.cpp \
    widget/CLedIndicatorHealt.cpp \
    preference/PreferenceMDSHostListModel.cpp \
    snapshot/SnapshotManager.cpp \
    snapshot/SnapshotTableModel.cpp \
    snapshot/NewSnapshot.cpp \
    snapshot/NodeListModel.cpp \
    snapshot/NodeInSnapshotTableModel.cpp \
    monitor/healt/HighlighterForChaosType.cpp \
    tree_group/TreeGroupManager.cpp \
    tree_group/DomainListModel.cpp \
    tree_group/GroupTreeItem.cpp \
    tree_group/GroupTreeModel.cpp \
    tree_group/AddNewDomain.cpp \
    widget/CNodeLogWidget.cpp \
    data/AttributeValueChangeSet.cpp \
    data/ChaosAbstractDataSetTableModel.cpp \
    data/ChaosAbstractListModel.cpp \
    data/ChaosAbstractTableModel.cpp \
    data/CommandListModel.cpp \
    data/CommandParameterTableModel.cpp \
    data/CommandTemplateListModel.cpp \
    data/DatasetAttributeListModel.cpp \
    data/FixedInputChannelDatasetTableModel.cpp \
    data/FixedOutputChannelDatasetTableModel.cpp \
    data/delegate/TwoLineInformationItem.cpp \
    data/delegate/TwoLineInformationListItemDelegate.cpp \
    api_async_processor/ApiSubmitter.cpp \
    widget/ChaosWidgetCompanion.cpp \
    data/LogDomainListModel.cpp \
    api_async_processor/ApiHandler.cpp \
    data/LogDataTableModel.cpp \
    log_browser/LogBrowser.cpp \
    data/LogEntryAdvancedSearchTableModel.cpp \
    widget/CNodeResourceWidget.cpp \
    widget/ChaosMonitorWidgetCompanion.cpp \
    widget/CNodeHealthLabel.cpp \
    widget/CControlUnitDatasetLabel.cpp \
    widget/StateImageIndicatorWidget.cpp \
    language_editor/ScriptEditor.cpp \
    script/ScriptManager.cpp \
    script/ScriptDescriptionWidget.cpp \
    data/ScriptListModel.cpp \
    script/CreateNewScriptDialog.cpp \
    data/EditableDatasetTableModel.cpp \
    data/editor/dataset/EditableDatasetTableModelEditDialog.cpp \
    data/delegate/ComboBoxDelegate.cpp \
    data/EditableScriptVariableTableModel.cpp \
    data/editor/script/EditableScriptVariableTableModelEditDialog.cpp \
    data/editor/common/EditableSubtypeListWidget.cpp \
    data/editor/common/EditableSubtypeListTableModel.cpp \
    script/editor/CodeEditor.cpp \
    GlobalServices.cpp \
    script/ScriptInstanceManagerWidget.cpp \
    data/ScriptInstanceListModel.cpp \
    utility/WidgetUtility.cpp \
    widget/ChaosStorageTypeWidget.cpp \
    widget/OnOffImageIndicator.cpp \
    widget/CStateVisiblePushButton.cpp \
    status_manager/StatusManager.cpp \
    widget/CQListView.cpp \
    widget/CCommandStatisticWidget.cpp \
    plot/AbstractBinaryPlotAdapter.cpp \
    plot/BufferPlot.cpp \
    widget/WidgetDynamicList.cpp \
    data/SearchNodeListModel.cpp \
    data/LiveDatasetTableModel.cpp \
    data/SnapshotDatasetNodeListModel.cpp \
    preference/PreferenceManager.cpp \
    data/ControlUnitStateVaribleListModel.cpp \
    utility/MessageUtility.cpp \
    error/ErrorManager.cpp \
    log_browser/ApplicationErrorLogging.cpp \
    data/ApplicationErrorLoggingTableModel.cpp \
    node/agent/AgentEditor.cpp \
    data/AgentNodeAssociatedListModel.cpp \
    node/agent/AgentNodeAssociationEditor.cpp \
    data/LogEntryTableModel.cpp \
    data/LogProcessEntriesTableModel.cpp \
    data/json/JsonTableModel.cpp \
    language_editor/JsonEditor.cpp \
    node/agent/AgentSetting.cpp \
    widget/CQTableView.cpp \
    widget/CPropertyPushButton.cpp \
    widget/CPropertyTextEdit.cpp \
    widget/CPopupWidgetContainer.cpp \
    preference/SelectNetworkDomain.cpp \
    external_lib/QtNodeEditor/Connection.cpp \
    external_lib/QtNodeEditor/ConnectionBlurEffect.cpp \
    external_lib/QtNodeEditor/ConnectionGeometry.cpp \
    external_lib/QtNodeEditor/ConnectionGraphicsObject.cpp \
    external_lib/QtNodeEditor/ConnectionPainter.cpp \
    external_lib/QtNodeEditor/ConnectionState.cpp \
    external_lib/QtNodeEditor/ConnectionStyle.cpp \
    external_lib/QtNodeEditor/DataModelRegistry.cpp \
    external_lib/QtNodeEditor/FlowItemInterface.cpp \
    external_lib/QtNodeEditor/FlowScene.cpp \
    external_lib/QtNodeEditor/FlowView.cpp \
    external_lib/QtNodeEditor/FlowViewStyle.cpp \
    external_lib/QtNodeEditor/Node.cpp \
    external_lib/QtNodeEditor/NodeConnectionInteraction.cpp \
    external_lib/QtNodeEditor/NodeDataModel.cpp \
    external_lib/QtNodeEditor/NodeGeometry.cpp \
    external_lib/QtNodeEditor/NodeGraphicsObject.cpp \
    external_lib/QtNodeEditor/NodePainter.cpp \
    external_lib/QtNodeEditor/NodeState.cpp \
    external_lib/QtNodeEditor/NodeStyle.cpp \
    external_lib/QtNodeEditor/Properties.cpp \
    external_lib/QtNodeEditor/StyleCollection.cpp \
    node/connection_manager/ConnectionEditor.cpp \
    node/connection_manager/model/ControlUnitNodeDataModel.cpp \
    language_editor/LuaLanguageSupport.cpp \
    language_editor/CLINGLanguageSupport.cpp \
    widget/StorageBurst.cpp

HEADERS  += \
    search/SearchNodeResult.h \
    presenter/PresenterWidget.h \
    api_async_processor/ApiAsyncProcessor.h \
    api_async_processor/ApiAsyncRunnable.h \
    MainController.h \
    node/unit_server/UnitServerEditor.h \
    node/control_unit/ControUnitInstanceEditor.h \
    node/control_unit/DriverDescriptionInputDialog.h \
    node/control_unit/AttributeValueRangeDialog.h \
    node/data_service/DataServiceEditor.h \
    node/data_service/CreateEditDataService.h \
    preference/PreferenceDialog.h \
    monitor/healt/HealtMonitorWidget.h \
    monitor/healt/HealtPresenterWidget.h \
    monitor/healt/HealtWidgetsListPresenteWidget.h \
    node/control_unit/ControlUnitEditor.h \
    logic/property_switch/MultiPropertyLogicSwitch.h \
    logic/property_switch/SwitchAggregator.h \
    node/control_unit/ControlUnitCommandTemplateEditor.h \
    node/control_unit/CommandTemplateInstanceEditor.h \
    data/CommandReader.h \
    data/CommandParameterReader.h \
    widget/CDSAttrQLineEdit.h \
    data/ChaosTypedAttributeValueSetter.h \
    external_lib/qcustomplot.h \
    data/AttributeReader.h \
    monitor/handler/healt/HealtHeartbeatHandler.h \
    monitor/handler/healt/HealtStatusHandler.h \
    monitor/handler/AbstractAttributeHandler.h \
    monitor/handler/MonitorInt32AttributeHandler.h \
    monitor/handler/MonitorBoolAttributeHandler.h \
    monitor/handler/MonitorDoubleAttributeHandler.h \
    monitor/handler/MonitorInt64AttributeHandler.h \
    monitor/handler/MonitorStringAttributeHandler.h \
    monitor/handler/handler.h \
    monitor/handler/healt/healt.h \
    monitor/handler/system/SystemControlUnitRunScheduleDelay.h \
    monitor/monitor.h \
    monitor/handler/system/system.h \
    monitor/handler/MonitorTSTaggetBoolAttributeHandler.h \
    monitor/handler/AbstractTSTaggedAttributeHandler.h \
    monitor/handler/MonitorTSTaggedInt32AttributeHandler.h \
    monitor/handler/MonitorTSTaggedBinaryAttributeHandler.h \
    monitor/handler/MonitorTSTaggedDoubleAttributeHandler.h \
    monitor/handler/MonitorTSTaggedInt64AttributeHandler.h \
    monitor/handler/MonitorTSTaggedStringAttributeHandler.h \
    data/ChaosByteArray.h \
    plot/NodeAttributePlotting.h \
    data/DatasetReader.h \
    data/DatasetAttributeReader.h \
    data/data.h \
    widget/CLedIndicatorHealt.h \
    monitor/handler/MonitorBinaryAttributeHandler.h \
    preference/PreferenceMDSHostListModel.h \
    snapshot/SnapshotManager.h \
    snapshot/SnapshotTableModel.h \
    snapshot/NewSnapshot.h \
    snapshot/NodeListModel.h \
    snapshot/NodeInSnapshotTableModel.h \
    monitor/healt/HighlighterForChaosType.h \
    tree_group/TreeGroupManager.h \
    tree_group/DomainListModel.h \
    tree_group/GroupTreeItem.h \
    tree_group/GroupTreeModel.h \
    tree_group/AddNewDomain.h \
    widget/CNodeLogWidget.h \
    data/AttributeValueChangeSet.h \
    data/ChaosAbstractDataSetTableModel.h \
    data/ChaosAbstractListModel.h \
    data/ChaosAbstractTableModel.h \
    data/CommandListModel.h \
    data/CommandParameterTableModel.h \
    data/CommandTemplateListModel.h \
    data/DatasetAttributeListModel.h \
    data/FixedInputChannelDatasetTableModel.h \
    data/FixedOutputChannelDatasetTableModel.h \
    data/delegate/TwoLineInformationItem.h \
    data/delegate/TwoLineInformationListItemDelegate.h \
    api_async_processor/ApiSubmitter.h \
    api_async_processor/ApiHandler.h \
    widget/ChaosWidgetCompanion.h \
    data/LogDomainListModel.h \
    data/LogDataTableModel.h \
    data/SearchPager.h\
    log_browser/LogBrowser.h \
    data/LogEntryAdvancedSearchTableModel.h \
    widget/CNodeResourceWidget.h \
    widget/ChaosMonitorWidgetCompanion.h \
    widget/CNodeHealthLabel.h \
    widget/CControlUnitDatasetLabel.h \
    widget/StateImageIndicatorWidget.h \
    language_editor/ScriptEditor.h \
    script/ScriptManager.h \
    script/ScriptDescriptionWidget.h \
    data/ScriptListModel.h \
    script/CreateNewScriptDialog.h \
    data/EditableDatasetTableModel.h \
    data/editor/dataset/EditableDatasetTableModelEditDialog.h \
    data/delegate/ComboBoxDelegate.h \
    data/EditableScriptVariableTableModel.h \
    data/editor/script/EditableScriptVariableTableModelEditDialog.h \
    data/editor/common/EditableSubtypeListWidget.h \
    data/editor/common/EditableSubtypeListTableModel.h \
    script/editor/CodeEditor.h \
    GlobalServices.h \
    script/ScriptInstanceManagerWidget.h \
    data/ScriptInstanceListModel.h \
    utility/WidgetUtility.h \
    widget/ChaosStorageTypeWidget.h \
    widget/OnOffImageIndicator.h \
    widget/CStateVisiblePushButton.h \
    status_manager/StatusManager.h \
    widget/CQListView.h \
    widget/CCommandStatisticWidget.h \
    plot/AbstractBinaryPlotAdapter.h \
    plot/BufferPlot.h \
    widget/WidgetDynamicList.h \
    metatypes.h \
    data/SearchNodeListModel.h \
    data/LiveDatasetTableModel.h \
    data/SnapshotDatasetNodeListModel.h \
    preference/PreferenceManager.h \
    preference/preference_types.h \
    data/ControlUnitStateVaribleListModel.h \
    utility/MessageUtility.h \
    error/ErrorManager.h \
    log_browser/ApplicationErrorLogging.h \
    data/ApplicationErrorLoggingTableModel.h \
    node/agent/AgentEditor.h \
    data/AgentNodeAssociatedListModel.h \
    node/agent/AgentNodeAssociationEditor.h \
    data/LogEntryTableModel.h \
    data/LogProcessEntriesTableModel.h \
    data/json/JsonTableModel.h \
    language_editor/JsonEditor.h \
    node/agent/AgentSetting.h \
    widget/CQTableView.h \
    widget/CPropertyPushButton.h\
    precomp_header.h \
    widget/CPropertyTextEdit.h \
    widget/CPopupWidgetContainer.h \
    preference/SelectNetworkDomain.h \
    external_lib/QtNodeEditor/Connection.hpp \
    external_lib/QtNodeEditor/ConnectionBlurEffect.hpp \
    external_lib/QtNodeEditor/ConnectionGeometry.hpp \
    external_lib/QtNodeEditor/ConnectionGraphicsObject.hpp \
    external_lib/QtNodeEditor/ConnectionPainter.hpp \
    external_lib/QtNodeEditor/ConnectionState.hpp \
    external_lib/QtNodeEditor/ConnectionStyle.hpp \
    external_lib/QtNodeEditor/DataModelRegistry.hpp \
    external_lib/QtNodeEditor/FlowItemInterface.hpp \
    external_lib/QtNodeEditor/FlowScene.hpp \
    external_lib/QtNodeEditor/FlowView.hpp \
    external_lib/QtNodeEditor/FlowViewStyle.hpp \
    external_lib/QtNodeEditor/Node.hpp \
    external_lib/QtNodeEditor/NodeConnectionInteraction.hpp \
    external_lib/QtNodeEditor/NodeData.hpp \
    external_lib/QtNodeEditor/NodeDataModel.hpp \
    external_lib/QtNodeEditor/NodeGeometry.hpp \
    external_lib/QtNodeEditor/NodeGraphicsObject.hpp \
    external_lib/QtNodeEditor/NodePainter.hpp \
    external_lib/QtNodeEditor/NodeState.hpp \
    external_lib/QtNodeEditor/NodeStyle.hpp \
    external_lib/QtNodeEditor/OperatingSystem.hpp \
    external_lib/QtNodeEditor/PortType.hpp \
    external_lib/QtNodeEditor/Properties.hpp \
    external_lib/QtNodeEditor/QStringStdHash.hpp \
    external_lib/QtNodeEditor/Serializable.hpp \
    external_lib/QtNodeEditor/Style.hpp \
    external_lib/QtNodeEditor/StyleCollection.hpp \
    node/connection_manager/ConnectionEditor.h \
    node/connection_manager/model/ControlUnitNodeDataModel.h \
    language_editor/LanguageEditorSupport.h \
    language_editor/LuaLanguageSupport.h \
    language_editor/CLINGLanguageSupport.h \
    widget/StorageBurst.h

FORMS    += \
    search/searchnoderesult.ui \
    node/unit_server/UnitServerEditor.ui \
    node/control_unit/ControUnitInstanceEditor.ui \
    node/control_unit/DriverDescriptionInputDialog.ui \
    node/control_unit/AttributeValueRangeDialog.ui \
    node/data_service/DataServiceEditor.ui \
    node/data_service/CreateEditDataService.ui \
    preference/PreferenceDialog.ui \
    monitor/healt/HealtPresenterWidget.ui \
    monitor/healt/HealtWidgetsListPresenteWidget.ui \
    node/control_unit/ControlUnitEditor.ui \
    node/control_unit/ControlUnitCommandTemplateEditor.ui \
    node/control_unit/CommandTemplateInstanceEditor.ui \
    plot/NodeAttributePlotting.ui \
    snapshot/SnapshotManager.ui \
    snapshot/NewSnapshot.ui \
    tree_group/TreeGroupManager.ui \
    tree_group/AddNewDomain.ui \
    widget/CNodeLogWidget.ui \
    log_browser/LogBrowser.ui \
    widget/CNodeResourceWidget.ui \
    language_editor/ScriptEditor.ui \
    script/ScriptManager.ui \
    script/ScriptDescriptionWidget.ui \
    script/CreateNewScriptDialog.ui \
    data/editor/dataset/EditableDatasetTableModelEditDialog.ui \
    data/editor/script/EditableScriptVariableTableModelEditDialog.ui \
    data/editor/common/EditableSubtypeListWidget.ui \
    script/ScriptInstanceManagerWidget.ui \
    widget/CCommandStatisticWidget.ui \
    plot/BufferPlot.ui \
    log_browser/ApplicationErrorLogging.ui \
    node/agent/AgentEditor.ui \
    node/agent/AgentNodeAssociationEditor.ui \
    language_editor/JsonEditor.ui \
    node/agent/AgentSetting.ui \
    widget/CPropertyTextEdit.ui \
    widget/CPopupWidgetContainer.ui \
    widget/ChaosStorageTypeWidget.ui \
    preference/SelectNetworkDomain.ui \
    node/connection_manager/ConnectionEditor.ui \
    widget/StorageBurst.ui

DISTFILES += \
    dark_orange.stylesheet \
    CMakeLists.txt
