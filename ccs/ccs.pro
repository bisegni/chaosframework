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

LIBS += -L$$PWD/../usr/local/lib

LIBS +=     -lchaos_common\
            -lchaos_metadata_service_client\
            -ljsoncpp\
            -lzmq

SOURCES += main.cpp\
    search/SearchNodeResult.cpp \
    search/SearchNode.cpp \
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
    widget/LedIndicatorWidget.cpp \
    node/control_unit/ControlUnitEditor.cpp \
    logic/property_switch/MultiPropertyLogicSwitch.cpp \
    logic/property_switch/SwitchAggregator.cpp \
    widget/list/delegate/TwoLineInformationListItemDelegate.cpp \
    widget/list/delegate/TwoLineInformationItem.cpp \
    node/control_unit/ControlUnitCommandTemplateEditor.cpp \
    node/control_unit/CommandTemplateInstanceEditor.cpp \
    data/CommandReader.cpp \
    data/CommandParameterReader.cpp \
    widget/CDSAttrQLineEdit.cpp \
    data/ChaosTypedAttributeValueSetter.cpp \
    external_lib/qcustomplot.cpp \
    data/AttributeReader.cpp \
    widget/ChaosLabel.cpp \
    widget/ChaosDatasetLabel.cpp \
    data/ChaosByteArray.cpp \
    plot/NodeAttributePlotting.cpp \
    data/DatasetReader.cpp \
    data/DatasetAttributeReader.cpp \
    widget/CLedIndicatorHealt.cpp \
    preference/PreferenceMDSHostListModel.cpp \
    widget/ChaosHealtLabel.cpp \
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
    data/FixedOutputChannelDatasetTableModel.cpp

HEADERS  += \
    search/SearchNodeResult.h \
    search/SearchNode.h \
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
    widget/LedIndicatorWidget.h \
    node/control_unit/ControlUnitEditor.h \
    logic/property_switch/MultiPropertyLogicSwitch.h \
    logic/property_switch/SwitchAggregator.h \
    widget/list/delegate/TwoLineInformationItem.h \
    widget/list/delegate/TwoLineInformationListItemDelegate.h \
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
    widget/ChaosDatasetLabel.h \
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
    widget/ChaosHealtLabel.h \
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
    data/FixedOutputChannelDatasetTableModel.h

FORMS    += \
    search/searchnoderesult.ui \
    search/searchnode.ui \
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
    widget/CNodeLogWidget.ui

DISTFILES += \
    dark_orange.stylesheet \
    CMakeLists.txt
