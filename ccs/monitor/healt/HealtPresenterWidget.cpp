#include "HealtPresenterWidget.h"
#include "ui_HealtPresenterWidget.h"
#include "../../node/control_unit/ControlUnitEditor.h"
#include "../../node/control_unit/ControUnitInstanceEditor.h"
#include "../../node/unit_server/UnitServerEditor.h"
#include "../../plot/NodeAttributePlotting.h"

#include <QTime>
#include <QDebug>
#include <QDateTime>

#include <ChaosMetadataServiceClient/ChaosMetadataServiceClient.h>

using namespace chaos::metadata_service_client;
using namespace chaos::metadata_service_client::api_proxy;
static const QString TAG_NODE_INFO = "tag_node_info";

HealtPresenterWidget::HealtPresenterWidget(const QString &node_to_check,
                                           QWidget *parent) :
    QFrame(parent),
    node_uid(node_to_check),
    api_submitter(this),
    ui(new Ui::HealtPresenterWidget){
    ui->setupUi(this);
    //enable contextual menu on the widget
    setContextMenuPolicy(Qt::ActionsContextMenu);

    ui->labelUID->setTextFormat(Qt::RichText);
    ui->labelUID->setText(node_uid);

    ui->ledIndicatorHealt->setNodeUID(node_uid);
    ui->ledIndicatorHealt->initChaosContent();

    ui->labelStatus->setHealthAttribute(CNodeHealthLabel::HealthOperationalState);
    ui->labelStatus->setNodeUID(node_uid);
    ui->labelStatus->initChaosContent();

    ui->pushButtonOpenNodeEditor->setEnabled(false);

    setFrameStyle(QFrame::StyledPanel | QFrame::Raised);
    setLineWidth(2);
    setMidLineWidth(2);

    //lauch api that permi to retrive the type of the node
    api_submitter.submitApiResult(TAG_NODE_INFO,
                                  GET_CHAOS_API_PTR(chaos::metadata_service_client::api_proxy::node::GetNodeDescription)->execute(node_uid.toStdString()));
    ui->nodeResourceWidget->setNodeUID(node_uid);
    ui->nodeResourceWidget->initChaosContent();
    ui->nodeResourceWidget->updateChaosContent();
}

HealtPresenterWidget::~HealtPresenterWidget() {
    ui->nodeResourceWidget->deinitChaosContent();
    ui->ledIndicatorHealt->deinitChaosContent();
    ui->labelStatus->deinitChaosContent();
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
        if(api_result->hasKey(chaos::NodeDefinitionKey::NODE_TYPE)) {
            //we have type
            type = QString::fromStdString(api_result->getStringValue(chaos::NodeDefinitionKey::NODE_TYPE));
            bool is_cu = (type.compare(chaos::NodeType::NODE_TYPE_CONTROL_UNIT) == 0);
            bool is_ds = (type.compare(chaos::NodeType::NODE_TYPE_UNIT_SERVER) == 0);
            if(is_cu ||
                    is_ds) {
                ui->pushButtonOpenNodeEditor->setEnabled((type.compare(chaos::NodeType::NODE_TYPE_CONTROL_UNIT) == 0) ||
                                                         (type.compare(chaos::NodeType::NODE_TYPE_UNIT_SERVER) == 0));
                //update string
                ui->labelUID->setText(node_uid);
                ui->labelNodeType->setText(type);
                if(is_cu) {
                    QAction *action = new QAction("Load", this);
                    addAction(action);
                    connect(action,
                            SIGNAL(triggered()),
                            this,
                            SLOT(cuContextualmenuTrigger()));
                    action = new QAction("Unload", this);
                    addAction(action);
                    connect(action,
                            SIGNAL(triggered()),
                            this,
                            SLOT(cuContextualmenuTrigger()));
                    action = new QAction("Init", this);
                    addAction(action);
                    connect(action,
                            SIGNAL(triggered()),
                            this,
                            SLOT(cuContextualmenuTrigger()));
                    action = new QAction("Deinit", this);
                    addAction(action);
                    connect(action,
                            SIGNAL(triggered()),
                            this,
                            SLOT(cuContextualmenuTrigger()));
                    action = new QAction("Start", this);
                    addAction(action);
                    connect(action,
                            SIGNAL(triggered()),
                            this,
                            SLOT(cuContextualmenuTrigger()));
                    action = new QAction("Stop", this);
                    addAction(action);
                    connect(action,
                            SIGNAL(triggered()),
                            this,
                            SLOT(cuContextualmenuTrigger()));
                    action = new QAction("Plot", this);
                    addAction(action);
                    connect(action,
                            SIGNAL(triggered()),
                            this,
                            SLOT(cuContextualmenuTrigger()));
                }
            }
        }
    }
}


void HealtPresenterWidget::cuContextualmenuTrigger() {
    QAction* cm_action = qobject_cast<QAction*>(sender());
    if(cm_action->text().compare("Load") == 0) {
        api_submitter.submitApiResult(QString("cu_load"),
                                      GET_CHAOS_API_PTR(unit_server::LoadUnloadControlUnit)->execute(node_uid.toStdString(),
                                                                                                     true));
    }else if(cm_action->text().compare("Unload") == 0) {
        api_submitter.submitApiResult(QString("cu_unload"),
                                      GET_CHAOS_API_PTR(unit_server::LoadUnloadControlUnit)->execute(node_uid.toStdString(),
                                                                                                     false));
    }else if(cm_action->text().compare("Init") == 0) {
        api_submitter.submitApiResult(QString("cu_init"),
                                      GET_CHAOS_API_PTR(control_unit::InitDeinit)->execute(node_uid.toStdString(),
                                                                                           true));
    }else if(cm_action->text().compare("Deinit") == 0) {
        api_submitter.submitApiResult(QString("cu_deinit"),
                                      GET_CHAOS_API_PTR(control_unit::InitDeinit)->execute(node_uid.toStdString(),
                                                                                           false));
    }else if(cm_action->text().compare("Start") == 0) {
        api_submitter.submitApiResult(QString("cu_start"),
                                      GET_CHAOS_API_PTR(control_unit::StartStop)->execute(node_uid.toStdString(),
                                                                                          true));
    }else if(cm_action->text().compare("Stop") == 0) {
        api_submitter.submitApiResult(QString("cu_stop"),
                                      GET_CHAOS_API_PTR(control_unit::StartStop)->execute(node_uid.toStdString(),
                                                                                          false));
    }else if(cm_action->text().compare("Plot") == 0) {
        NodeAttributePlotting *plot_viewer = new NodeAttributePlotting(node_uid, NULL);
        plot_viewer->show();
    }
}
