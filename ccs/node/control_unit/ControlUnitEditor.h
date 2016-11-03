#ifndef CONTROLUNITEDITOR_H
#define CONTROLUNITEDITOR_H
#include "ControlUnitCommandTemplateEditor.h"

#include "../../data/CommandListModel.h"
#include "../../data/CommandTemplateListModel.h"
#include "../../data/FixedInputChannelDatasetTableModel.h"
#include "../../data/FixedOutputChannelDatasetTableModel.h"

#include "../../monitor/monitor.h"
#include "../../presenter/PresenterWidget.h"
#include "../../logic/property_switch/SwitchAggregator.h"
#include "../../widget/CLedIndicatorHealt.h"

#include <QWidget>
#include <QItemSelection>

namespace Ui {
class ControlUnitEditor;
}

class ControlUnitEditor :
        public PresenterWidget,
        chaos::metadata_service_client::node_monitor::NodeMonitorHandler {
    Q_OBJECT

public:
    explicit ControlUnitEditor(const QString& _control_unit_unique_id);
    ~ControlUnitEditor();
protected:
    void initUI();
    bool isClosing();
    void onApiDone(const QString& tag,
                   QSharedPointer<chaos::common::data::CDataWrapper> api_result);

    void nodeChangedOnlineState(const std::string& node_uid,
                                chaos::metadata_service_client::node_monitor::OnlineState old_state,
                                chaos::metadata_service_client::node_monitor::OnlineState new_state);

    void nodeChangedInternalState(const std::string& node_uid,
                                  const std::string& old_state,
                                  const std::string& new_state);

    void nodeHasBeenRestarted(const std::string& node_uid);
private slots:

    void onLogicSwitchChangeState(const QString& switch_name,
                                  bool switch_activate);

    void handleSelectionChangedOnListWiew(const QItemSelection& selection,
                                          const QItemSelection& previous_selected);

    void templateSaved(const QString& tempalte_name,
                       const QString& command_uid);

    void on_pushButtonLoadAction_clicked();

    void on_pushButtonUnload_clicked();

    void on_pushButtonInitAction_clicked();

    void on_pushButtonDeinitAction_clicked();

    void on_pushButtonStartAction_clicked();

    void on_pushButtonStopAction_clicked();

    void on_checkBoxMonitorOutputChannels_clicked();

    void on_checkBoxMonitorInputChannels_clicked();

    void on_pushButtonCommitSet_clicked();

    void on_pushButtonResetChangeSet_clicked();

    void on_pushButtonAddNewCommadInstance_clicked();

    void on_pushButtonUpdateTemaplteList_clicked();

    void on_pushButtonEditInstance_clicked();

    void on_pushButtonRemoveInstance_clicked();

    void on_pushButtonCreateInstance_clicked();

    void on_pushButtonSetRunScheduleDelay_clicked();

    void on_pushButtonRecoverError_clicked();

    void on_pushButtonOpenInstanceEditor_clicked();

    void tabIndexChanged(int new_index);

    void on_pushButtonShowPlot_clicked();

private:
    QString getStatusString(int status);

    void updateTemplateSearch();
    void updateAllControlUnitInfomration();
    void manageMonitoring(bool start);
    void fillInfo(const QSharedPointer<chaos::common::data::CDataWrapper>& node_info);
    void fillDataset(const QSharedPointer<chaos::common::data::CDataWrapper>& dataset);

    //keep track of the last relevated online state
    bool restarted;
    bool last_online_state;
    const QString           control_unit_unique_id;
    QString                 unit_server_parent_unique_id;
    SwitchAggregator        logic_switch_aggregator;

    CommandListModel                    command_list_model;
    CommandTemplateListModel            command_template_list_model;

    FixedOutputChannelDatasetTableModel dataset_output_table_model;
    FixedInputChannelDatasetTableModel  dataset_input_table_model;

    Ui::ControlUnitEditor *ui;
};

#endif // CONTROLUNITEDITOR_H
