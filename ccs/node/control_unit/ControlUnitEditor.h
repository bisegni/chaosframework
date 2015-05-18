#ifndef CONTROLUNITEDITOR_H
#define CONTROLUNITEDITOR_H

#include "../../presenter/PresenterWidget.h"
#include "../../monitor/healt/handler/handler.h"
#include "../data/FixedOutputChannelDatasetTableModel.h"

#include <QWidget>

namespace Ui {
class ControlUnitEditor;
}

class ControlUnitEditor :
        public PresenterWidget
{
    Q_OBJECT

public:
    explicit ControlUnitEditor(const QString& _control_unit_unique_id);
    ~ControlUnitEditor();
protected:
    void updateAll();
    void initUI();
    bool canClose();
    void onApiDone(const QString& tag,
                   QSharedPointer<chaos::common::data::CDataWrapper> api_result);

private slots:
    void updateAttributeValue(const QString& attribute_name,
                              const QVariant& attribute_value);
private:
    void fillInfo(const QSharedPointer<chaos::common::data::CDataWrapper>& node_info);
    void fillDataset(const QSharedPointer<chaos::common::data::CDataWrapper>& dataset);

    HealthHartbeatHandler   hb_handler;
    HealtStatusHandler      status_handler;
    FixedOutputChannelDatasetTableModel channel_output_table_model;
    const QString control_unit_unique_id;
    Ui::ControlUnitEditor *ui;
};

#endif // CONTROLUNITEDITOR_H
