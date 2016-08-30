#-------------------------------------------------
#
# Project created by QtCreator 2015-03-31T11:15:31
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TARGET = ccs
TEMPLATE = app
RESOURCES = qdarkstyle/style.qrc \
    dark_orange/dark_orange_style.qrc \
    image/image.qrc

INCLUDEPATH += $$PWD/../
INCLUDEPATH += $$PWD/../usr/local/include

LIBS += -L$$_PRO_FILE_PWD_/../usr/local/lib

unix:!macx {
    LIBS +=  -Wl,--whole-archive -lchaos_common -Wl,--no-whole-archive
}

macx:{
#    ICON = ccs_icon.icns
    LIBS +=  -lchaos_common
}

LIBS +=     -lchaos_metadata_service_client\
            -ljsoncpp\
            -lzmq\
            -lpthread\
            -lmongoose

SOURCES += main.cpp\
    search/SearchNodeResult.cpp \
    presenter/PresenterWidget.cpp \
    presenter/CommandPresenter.cpp \
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
    MainWindow.cpp \
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
    widget/ChaosLabel.cpp \
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
    data/LogEntryTableModel.cpp \
    api_async_processor/ApiHandler.cpp \
    data/LogDataTableModel.cpp \
    log_browser/LogBrowser.cpp \
    data/LogEntryAdvancedSearchTableModel.cpp \
    widget/CNodeResourceWidget.cpp \
    widget/ChaosMonitorWidgetCompanion.cpp \
    widget/CNodeHealthLabel.cpp \
    widget/CControlUnitDatasetLabel.cpp \
    widget/StateImageIndicatorWidget.cpp \
    language_editor/LuaHighlighter.cpp \
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
    utility/WidgetUtility.cpp

HEADERS  += \
    search/SearchNodeResult.h \
    presenter/PresenterWidget.h \
    presenter/CommandPresenter.h \
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
    MainWindow.h \
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
    widget/ChaosLabel.h \
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
    data/LogEntryTableModel.h \
    data/LogDataTableModel.h \
    data/SearchPager.h\
    log_browser/LogBrowser.h \
    data/LogEntryAdvancedSearchTableModel.h \
    widget/CNodeResourceWidget.h \
    widget/ChaosMonitorWidgetCompanion.h \
    widget/CNodeHealthLabel.h \
    widget/CControlUnitDatasetLabel.h \
    widget/StateImageIndicatorWidget.h \
    language_editor/LuaHighlighter.h \
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
    utility/WidgetUtility.h

FORMS    += \
    search/searchnoderesult.ui \
    node/unit_server/UnitServerEditor.ui \
    node/control_unit/ControUnitInstanceEditor.ui \
    node/control_unit/DriverDescriptionInputDialog.ui \
    node/control_unit/AttributeValueRangeDialog.ui \
    node/data_service/DataServiceEditor.ui \
    node/data_service/CreateEditDataService.ui \
    preference/PreferenceDialog.ui \
    MainWindow.ui \
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
    script/ScriptInstanceManagerWidget.ui

DISTFILES += \
    dark_orange.stylesheet \
    CMakeLists.txt
