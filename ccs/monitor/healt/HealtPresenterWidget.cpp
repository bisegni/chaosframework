#include "HealtPresenterWidget.h"
#include "ui_HealtPresenterWidget.h"
#include "../../node/control_unit/ControlUnitEditor.h"
#include "../../node/control_unit/ControUnitInstanceEditor.h"
#include "../../node/unit_server/UnitServerEditor.h"
#include "../../plot/NodeAttributePlotting.h"

#include <QTime>
#include <QDebug>
#include <QDateTime>

using namespace chaos::metadata_service_client;
using namespace chaos::metadata_service_client::api_proxy;
static const QString TAG_NODE_INFO = "tag_node_info";

HealtPresenterWidget::HealtPresenterWidget(const QString &node_to_check,
                                           QWidget *parent) :
    QFrame(parent),
    is_cu(false),
    is_sc_cu(false),
    is_ds(false),
    wUtil(this),
    node_uid(node_to_check),
    api_submitter(this),
    ui(new Ui::HealtPresenterWidget){
    ui->setupUi(this);

    ui->labelUID->setTextFormat(Qt::RichText);
    ui->labelUID->setText(node_uid);

    ui->ledIndicatorHealt->setNodeUID(node_uid);
    ui->ledIndicatorHealt->initChaosContent();
    connect(ui->ledIndicatorHealt,
            SIGNAL(changedOnlineStatus(QString,chaos::metadata_service_client::node_monitor::OnlineState)),
            SLOT(changedOnlineStatus(QString,chaos::metadata_service_client::node_monitor::OnlineState)));
    ui->labelStatus->setHealthAttribute(CNodeHealthLabel::HealthOperationalState);
    ui->labelStatus->setNodeUID(node_uid);
    ui->labelStatus->initChaosContent();

    ui->pushButtonOpenNodeEditor->setEnabled(false);

    setFrameStyle(QFrame::StyledPanel | QFrame::Raised);
    setLineWidth(2);
    setMidLineWidth(2);

    ui->nodeResourceWidget->setNodeUID(node_uid);
    ui->nodeResourceWidget->initChaosContent();
    ui->nodeResourceWidget->updateChaosContent();

    ui->widgetCommandStatistic->setNodeUID(node_uid);

    //force start refresh also if node is down
    changedOnlineStatus(node_uid,
                        chaos::metadata_service_client::node_monitor::OnlineStateON);
    QVector< QPair<QString, QVariant> > cm_map;
    cm_map.push_back(QPair<QString, QVariant>("Load", QVariant()));
    cm_map.push_back(QPair<QString, QVariant>("Init", QVariant()));
    cm_map.push_back(QPair<QString, QVariant>("Start", QVariant()));
    cm_map.push_back(QPair<QString, QVariant>("Stop", QVariant()));
    cm_map.push_back(QPair<QString, QVariant>("Deinit", QVariant()));
    cm_map.push_back(QPair<QString, QVariant>("Unload", QVariant()));
    cm_map.push_back(QPair<QString, QVariant>("Plot", QVariant()));
    wUtil.cmRegisterActions(this,
                            cm_map);
    node_action = wUtil.cmGetAction(this, "action");
    chaos::metadata_service_client::ChaosMetadataServiceClient::getInstance()->addHandlerToNodeMonitor(node_uid.toStdString(),
                                                                                                       node_monitor::ControllerTypeNode,
                                                                                                       this);
}

HealtPresenterWidget::~HealtPresenterWidget() {
    ui->nodeResourceWidget->deinitChaosContent();
    ui->ledIndicatorHealt->deinitChaosContent();
    ui->labelStatus->deinitChaosContent();
    chaos::metadata_service_client::ChaosMetadataServiceClient::getInstance()->removeHandlerToNodeMonitor(node_uid.toStdString(),
                                                                                                          node_monitor::ControllerTypeNode,
                                                                                                          this);
    delete ui;
}

void HealtPresenterWidget::on_pushButtonOpenNodeEditor_clicked() {
    QWidget *w_to_open = NULL;
    if(type.compare(chaos::NodeType::NODE_TYPE_CONTROL_UNIT) == 0) {
        w_to_open = new ControlUnitEditor(node_uid);
    } else if(type.compare(chaos::NodeType::NODE_TYPE_UNIT_SERVER) == 0) {
        w_to_open = new UnitServerEditor(node_uid);
    }
    if(w_to_open) w_to_open->show();
}

//!Api has ben called successfully
void HealtPresenterWidget::onApiDone(const QString& api_tag,
                                     QSharedPointer<chaos::common::data::CDataWrapper> api_result) {
    if(api_tag.compare(TAG_NODE_INFO) == 0) {
        //we have faound the node description
        if(api_result->hasKey(chaos::NodeDefinitionKey::NODE_TYPE) &&
                api_result->hasKey(chaos::NodeDefinitionKey::NODE_SUB_TYPE)) {
            //we have type
            type = QString::fromStdString(api_result->getStringValue(chaos::NodeDefinitionKey::NODE_TYPE));
            subtype =  QString::fromStdString(api_result->getStringValue(chaos::NodeDefinitionKey::NODE_SUB_TYPE));
            is_cu = (type.compare(chaos::NodeType::NODE_TYPE_CONTROL_UNIT) == 0);
            is_sc_cu = subtype.compare(chaos::CUType::SCCU) == 0;
            is_ds = (type.compare(chaos::NodeType::NODE_TYPE_UNIT_SERVER) == 0);

            ui->widgetCommandStatistic->setVisible(is_sc_cu);
            if(ui->widgetCommandStatistic->isVisible()) {
                ui->widgetCommandStatistic->initChaosContent();
            } else {
                ui->widgetCommandStatistic->deinitChaosContent();
            }

            if(is_cu ||
                    is_ds) {
                ui->pushButtonOpenNodeEditor->setEnabled((type.compare(chaos::NodeType::NODE_TYPE_CONTROL_UNIT) == 0) ||
                                                         (type.compare(chaos::NodeType::NODE_TYPE_UNIT_SERVER) == 0));
                //update string
                ui->labelUID->setText(node_uid);
                ui->labelNodeType->setText(QString("%1[%2]").arg(type).arg(subtype));
            }
        }
    }
}


void HealtPresenterWidget::nodeChangedOnlineState(const std::string& node_uid,
                                                  chaos::metadata_service_client::node_monitor::OnlineState old_state,
                                                  chaos::metadata_service_client::node_monitor::OnlineState new_state) {

}

void HealtPresenterWidget::nodeChangedInternalState(const std::string& node_uid,
                                                    const std::string& old_state,
                                                    const std::string& new_state) {

}

void HealtPresenterWidget::nodeHasBeenRestarted(const std::string& node_uid) {

}

void HealtPresenterWidget::cmActionTrigger(const QString& cm_title,
                                           const QVariant& cm_data) {
    if(cm_title.compare("Load") == 0) {
        api_submitter.submitApiResult(QString("cu_load"),
                                      GET_CHAOS_API_PTR(unit_server::LoadUnloadControlUnit)->execute(node_uid.toStdString(),
                                                                                                     true));
    } else if(cm_title.compare("Unload") == 0) {
        api_submitter.submitApiResult(QString("cu_unload"),
                                      GET_CHAOS_API_PTR(unit_server::LoadUnloadControlUnit)->execute(node_uid.toStdString(),
                                                                                                     false));
    } else if(cm_title.compare("Unload") == 0) {
        api_submitter.submitApiResult(QString("cu_unload"),
                                      GET_CHAOS_API_PTR(unit_server::LoadUnloadControlUnit)->execute(node_uid.toStdString(),
                                                                                                     false));
    } else if(cm_title.compare("Init") == 0) {
        api_submitter.submitApiResult(QString("cu_init"),
                                      GET_CHAOS_API_PTR(control_unit::InitDeinit)->execute(node_uid.toStdString(),
                                                                                           true));
    } else if(cm_title.compare("Deinit") == 0) {
        api_submitter.submitApiResult(QString("cu_deinit"),
                                      GET_CHAOS_API_PTR(control_unit::InitDeinit)->execute(node_uid.toStdString(),
                                                                                           false));
    } else if(cm_title.compare("Start") == 0) {
        api_submitter.submitApiResult(QString("cu_start"),
                                      GET_CHAOS_API_PTR(control_unit::StartStop)->execute(node_uid.toStdString(),
                                                                                          true));
    } else if(cm_title.compare("Stop") == 0) {
        api_submitter.submitApiResult(QString("cu_stop"),
                                      GET_CHAOS_API_PTR(control_unit::StartStop)->execute(node_uid.toStdString(),
                                                                                          false));
    } else if(cm_title.compare("Plot") == 0) {
        NodeAttributePlotting *plot_viewer = new NodeAttributePlotting(node_uid, NULL);
        plot_viewer->show();
    }
}

void HealtPresenterWidget::changedOnlineStatus(const QString& node_uid,
                                               chaos::metadata_service_client::node_monitor::OnlineState online_state) {
    if(online_state == chaos::metadata_service_client::node_monitor::OnlineStateON) {
        //lauch api that permi to retrive the type of the node
        api_submitter.submitApiResult(TAG_NODE_INFO,
                                      GET_CHAOS_API_PTR(chaos::metadata_service_client::api_proxy::node::GetNodeDescription)->execute(node_uid.toStdString()));
    }
}
