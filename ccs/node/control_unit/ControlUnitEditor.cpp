#include "ControlUnitEditor.h"
#include "ui_controluniteditor.h"

#include <QDateTime>

static const QString TAG_CU_INFO = QString("g_cu_i");
static const QString TAG_CU_DATASET = QString("g_cu_d");

using namespace chaos::metadata_service_client;
using namespace chaos::metadata_service_client::api_proxy;
ControlUnitEditor::ControlUnitEditor(const QString &_control_unit_unique_id) :
    PresenterWidget(NULL),
    control_unit_unique_id(_control_unit_unique_id),
    ui(new Ui::ControlUnitEditor),
    channel_output_table_model(NULL){
    ui->setupUi(this);
    //handler connection
    connect(&status_handler,
            SIGNAL(valueUpdated(QString,QVariant)),
            SLOT(updateAttributeValue(QString,QVariant)));
    connect(&hb_handler,
            SIGNAL(valueUpdated(QString,QVariant)),
            SLOT(updateAttributeValue(QString,QVariant)));
}

ControlUnitEditor::~ControlUnitEditor() {
    ChaosMetadataServiceClient::getInstance()->removeKeyAttributeHandlerForHealt(control_unit_unique_id.toStdString(),
                                                                              20,
                                                                              &status_handler);
    ChaosMetadataServiceClient::getInstance()->removeKeyAttributeHandlerForHealt(control_unit_unique_id.toStdString(),
                                                                              20,
                                                                              &hb_handler);
    delete ui;
}

void ControlUnitEditor::updateAll() {
    submitApiResult(QString(TAG_CU_INFO),
                    GET_CHAOS_API_PTR(node::GetNodeDescription)->execute(control_unit_unique_id.toStdString()));
    submitApiResult(QString(TAG_CU_DATASET),
                    GET_CHAOS_API_PTR(control_unit::GetCurrentDataset)->execute(control_unit_unique_id.toStdString()));
}

void ControlUnitEditor::updateAttributeValue(const QString& attribute_name,
                                             const QVariant& attribute_value) {
    if(attribute_name.compare(chaos::NodeHealtDefinitionKey::NODE_HEALT_STATUS) == 0) {
        //print the status
        ui->labelhealtStatus->setText(attribute_value.toString());
    } else if(attribute_name.compare(chaos::NodeHealtDefinitionKey::NODE_HEALT_TIMESTAMP) == 0){
        //print the timestamp and update the red/green indicator
        if(attribute_value.isNull()) {
            ui->ledIndicatorStatus->setState(0);
        }else {
            quint64 time_diff = QDateTime::currentDateTimeUtc().currentMSecsSinceEpoch() - attribute_value.toULongLong();
            if(time_diff <= 5000) {
                //in time
                ui->ledIndicatorStatus->setState(2);
            } else {
                //timeouted
                ui->ledIndicatorStatus->setState(1);
            }
        }
    }
}

void ControlUnitEditor::initUI() {
    //add model to table
    ui->tableViewOutputChannel->setModel(&channel_output_table_model);
    ui->tableViewOutputChannel->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    QSharedPointer<QIcon> not_found_icon(new QIcon(":/images/white_circle_indicator.png"));
    QSharedPointer<QIcon> offline_icon(new QIcon(":/images/red_circle_indicator.png"));
    QSharedPointer<QIcon> on_icon(new QIcon(":/images/green_circle_indicator.png"));

    ui->ledIndicatorStatus->addState(0, not_found_icon);
    ui->ledIndicatorStatus->addState(1, offline_icon);
    ui->ledIndicatorStatus->addState(2, on_icon);

    ChaosMetadataServiceClient::getInstance()->addKeyAttributeHandlerForHealt(control_unit_unique_id.toStdString(),
                                                                              20,
                                                                              &status_handler);
    ChaosMetadataServiceClient::getInstance()->addKeyAttributeHandlerForHealt(control_unit_unique_id.toStdString(),
                                                                              20,
                                                                              &hb_handler);

    //launch api for control unit information
    updateAll();
}

bool ControlUnitEditor::canClose() {
    return true;
}

void ControlUnitEditor::onApiDone(const QString& tag,
                                  QSharedPointer<chaos::common::data::CDataWrapper> api_result) {
    if(tag.compare(TAG_CU_INFO) == 0) {
        fillInfo(api_result);
    } else if(tag.compare(TAG_CU_DATASET) == 0) {
        fillDataset(api_result);
    }
}

void ControlUnitEditor::fillInfo(const QSharedPointer<chaos::common::data::CDataWrapper>& node_info) {
    if(node_info->hasKey(chaos::NodeDefinitionKey::NODE_UNIQUE_ID)) {
        ui->labelUniqueIdentifier->setText(QString::fromStdString(node_info->getStringValue(chaos::NodeDefinitionKey::NODE_UNIQUE_ID)));
    }

    if(node_info->hasKey(chaos::NodeDefinitionKey::NODE_RPC_ADDR)) {
        ui->labelRemoteAddress->setText(QString::fromStdString(node_info->getStringValue(chaos::NodeDefinitionKey::NODE_RPC_ADDR)));
    }
}

void ControlUnitEditor::fillDataset(const QSharedPointer<chaos::common::data::CDataWrapper>& dataset) {
    channel_output_table_model.updateData(dataset);
}
