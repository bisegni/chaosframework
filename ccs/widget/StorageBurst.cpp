#include "StorageBurst.h"
#include "ui_StorageBurst.h"
#include <chaos/common/data/CDataVariant.h>
#include <chaos/common/data/structured/Dataset.h>

using namespace chaos::common::data;
using namespace chaos::common::data::structured;

StorageBurst::StorageBurst(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::StorageBurst) {
    ui->setupUi(this);
    ui->comboBoxType->clear();
    ui->comboBoxType->addItem("Push", QVariant(chaos::ControlUnitNodeDefinitionType::DSStorageBurstTypeNPush));
    ui->comboBoxType->addItem("Millliseconds", QVariant(chaos::ControlUnitNodeDefinitionType::DSStorageBurstTypeMSec));
}

StorageBurst::~StorageBurst() {
    delete ui;
}

void StorageBurst::on_comboBox_currentIndexChanged(const QString &selected_item) {
    if(selected_item.compare("Push") == 0) {
        ui->lineEditValue->setPlaceholderText("Number of pushes[up to 10000]");
        ui->lineEditValue->setValidator(new QIntValidator(1, 10000, ui->lineEditValue));
    } else if(selected_item.compare("Timed")) {
        ui->lineEditValue->setPlaceholderText("Number of milliseconds[up to 1h]");
        ui->lineEditValue->setValidator(new QIntValidator(1, 1000*60*60, ui->lineEditValue));
    }
}

void StorageBurst::apiHasStarted(const QString& api_tag) {
    setEnabled(false);
}

void StorageBurst::apiHasEnded(const QString& api_tag) {
    setEnabled(true);
}

void StorageBurst::apiHasEndedWithError(const QString& api_tag,
                                        QSharedPointer<chaos::CException> api_exception) {
    setEnabled(true);
}

void StorageBurst::on_pushButtonSubmit_clicked() {
    chaos::common::data::structured::DatasetBurst ds_burst;
    ds_burst.type = static_cast<chaos::ControlUnitNodeDefinitionType::DSStorageBurstType>(ui->comboBoxType->currentData().toInt());
    ds_burst.value = CDataVariant(ui->lineEditValue->text().toInt());
    ds_burst.tag = ui->lineEditTagName->text().toStdString();
    submitApiResult("submit_burst",
                    GET_CHAOS_API_PTR(chaos::metadata_service_client::api_proxy::control_unit::SendStorageBurst)->execute(nodeUID().toStdString(),
                                                                                                                          ds_burst));
}
