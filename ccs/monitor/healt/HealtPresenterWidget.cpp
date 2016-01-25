#include "HealtPresenterWidget.h"
#include "ui_HealtPresenterWidget.h"
#include "../../node/control_unit/ControlUnitEditor.h"
#include "../../node/control_unit/ControUnitInstanceEditor.h"
#include "../../node/unit_server/UnitServerEditor.h"
#include "../../plot/NodeAttributePlotting.h"

#include <QDateTime>
#include <QDebug>

#include <ChaosMetadataServiceClient/ChaosMetadataServiceClient.h>

using namespace chaos::metadata_service_client;
using namespace chaos::metadata_service_client::api_proxy;
static const QString TAG_NODE_INFO = "tag_node_info";

HealtPresenterWidget::HealtPresenterWidget(CommandPresenter *_global_command_presenter,
                                           const QString &node_to_check,
                                           QWidget *parent) :
    QFrame(parent),
    ui(new Ui::HealtPresenterWidget),
    global_command_presenter(_global_command_presenter),
    node_uid(node_to_check){
    ui->setupUi(this);
    //enable contextual menu on the widget
    setContextMenuPolicy(Qt::ActionsContextMenu);

    ui->labelUID->setTextFormat(Qt::RichText);
    ui->labelUID->setText(node_uid);

    ui->ledIndicatorHealt->setNodeUniqueID(node_uid);
    ui->ledIndicatorHealt->startMonitoring();

    ui->labelStatus->setNodeUniqueID(node_uid);
    ui->labelStatus->setTrackStatus(true);
    ui->labelStatus->setLabelValueShowTrackStatus(true);
    ui->labelStatus->startMonitoring();
    connect(ui->labelStatus,
            SIGNAL(statusChanged(QString,chaos::metadata_service_client::monitor_system::KeyValue)),
            SLOT(statusChanged(QString,chaos::metadata_service_client::monitor_system::KeyValue)));
    connect(ui->labelStatus,
            SIGNAL(statusNoData(QString)),
            SLOT(statusNoData(QString)));

    ui->pushButtonOpenNodeEditor->setEnabled(false);

    setFrameStyle(QFrame::StyledPanel | QFrame::Raised);
    setLineWidth(2);
    setMidLineWidth(2);

    //lauch api that permi to retrive the type of the node
    api_processor.submitApiResult(TAG_NODE_INFO,
                                  GET_CHAOS_API_PTR(chaos::metadata_service_client::api_proxy::node::GetNodeDescription)->execute(node_uid.toStdString()),
                                  this,
                                  SLOT(asyncApiResult(QString, QSharedPointer<chaos::common::data::CDataWrapper>)),
                                  SLOT(asyncApiError(QString, QSharedPointer<chaos::CException>)),
                                  SLOT(asyncApiTimeout(QString)));

}

HealtPresenterWidget::~HealtPresenterWidget() {
    ui->ledIndicatorHealt->stopMonitoring();
    ui->labelStatus->stopMonitoring();
    delete ui;
}

void HealtPresenterWidget::on_pushButtonOpenNodeEditor_clicked() {
    CHAOS_ASSERT(global_command_presenter);
    if(type.compare(chaos::NodeType::NODE_TYPE_CONTROL_UNIT) == 0) {
        global_command_presenter->showCommandPresenter(new ControlUnitEditor(node_uid));
    } else if(type.compare(chaos::NodeType::NODE_TYPE_UNIT_SERVER) == 0) {
        global_command_presenter->showCommandPresenter(new UnitServerEditor(node_uid));
    }
}

//!Api has ben called successfully
void HealtPresenterWidget::asyncApiResult(const QString& api_tag,
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
                ui->labelUID->setText(QString("%1-<font color=\"#4EB66B\">[%2]</font>").arg(node_uid, type));
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
        api_processor.submitApiResult(QString("cu_load"),
                                      GET_CHAOS_API_PTR(unit_server::LoadUnloadControlUnit)->execute(node_uid.toStdString(),
                                                                                                     true),
                                      this,
                                      SLOT(asyncApiResult(QString, QSharedPointer<chaos::common::data::CDataWrapper>)),
                                      SLOT(asyncApiError(QString, QSharedPointer<chaos::CException>)),
                                      SLOT(asyncApiTimeout(QString)));
    }else if(cm_action->text().compare("Unload") == 0) {
        api_processor.submitApiResult(QString("cu_unload"),
                                      GET_CHAOS_API_PTR(unit_server::LoadUnloadControlUnit)->execute(node_uid.toStdString(),
                                                                                                     false),
                                      this,
                                      SLOT(asyncApiResult(QString, QSharedPointer<chaos::common::data::CDataWrapper>)),
                                      SLOT(asyncApiError(QString, QSharedPointer<chaos::CException>)),
                                      SLOT(asyncApiTimeout(QString)));
    }else if(cm_action->text().compare("Init") == 0) {
        api_processor.submitApiResult(QString("cu_init"),
                                      GET_CHAOS_API_PTR(control_unit::InitDeinit)->execute(node_uid.toStdString(),
                                                                                           true),
                                      this,
                                      SLOT(asyncApiResult(QString, QSharedPointer<chaos::common::data::CDataWrapper>)),
                                      SLOT(asyncApiError(QString, QSharedPointer<chaos::CException>)),
                                      SLOT(asyncApiTimeout(QString)));
    }else if(cm_action->text().compare("Deinit") == 0) {
        api_processor.submitApiResult(QString("cu_deinit"),
                                      GET_CHAOS_API_PTR(control_unit::InitDeinit)->execute(node_uid.toStdString(),
                                                                                           false),
                                      this,
                                      SLOT(asyncApiResult(QString, QSharedPointer<chaos::common::data::CDataWrapper>)),
                                      SLOT(asyncApiError(QString, QSharedPointer<chaos::CException>)),
                                      SLOT(asyncApiTimeout(QString)));
    }else if(cm_action->text().compare("Start") == 0) {
        api_processor.submitApiResult(QString("cu_start"),
                                      GET_CHAOS_API_PTR(control_unit::StartStop)->execute(node_uid.toStdString(),
                                                                                          true),
                                      this,
                                      SLOT(asyncApiResult(QString, QSharedPointer<chaos::common::data::CDataWrapper>)),
                                      SLOT(asyncApiError(QString, QSharedPointer<chaos::CException>)),
                                      SLOT(asyncApiTimeout(QString)));
    }else if(cm_action->text().compare("Stop") == 0) {
        api_processor.submitApiResult(QString("cu_stop"),
                                      GET_CHAOS_API_PTR(control_unit::StartStop)->execute(node_uid.toStdString(),
                                                                                          false),
                                      this,
                                      SLOT(asyncApiResult(QString, QSharedPointer<chaos::common::data::CDataWrapper>)),
                                      SLOT(asyncApiError(QString, QSharedPointer<chaos::CException>)),
                                      SLOT(asyncApiTimeout(QString)));
    }else if(cm_action->text().compare("Plot") == 0) {
        NodeAttributePlotting *plot_viewer = new NodeAttributePlotting(node_uid, NULL);
        plot_viewer->show();
    }
}

//!Api has been give an error
void HealtPresenterWidget::asyncApiError(const QString& tag,
                                         QSharedPointer<chaos::CException> api_exception) {

}

void HealtPresenterWidget::asyncApiTimeout(const QString& tag) {

}

void HealtPresenterWidget::statusChanged(const QString& node_uid,
                                         const chaos::metadata_service_client::monitor_system::KeyValue& healt_values) {
    if(healt_values->hasKey(chaos::NodeHealtDefinitionKey::NODE_HEALT_USER_TIME)){
        ui->labelUsrProc->setText(QString::number(healt_values->getDoubleValue(chaos::NodeHealtDefinitionKey::NODE_HEALT_USER_TIME), 'f', 1 ));
    }
    if(healt_values->hasKey(chaos::NodeHealtDefinitionKey::NODE_HEALT_SYSTEM_TIME)){
        ui->labelSysProc->setText(QString::number(healt_values->getDoubleValue(chaos::NodeHealtDefinitionKey::NODE_HEALT_SYSTEM_TIME), 'f', 1 ));
    }
    if(healt_values->hasKey(chaos::NodeHealtDefinitionKey::NODE_HEALT_PROCESS_SWAP)){
        ui->labelSwapProc->setText(QString::number(healt_values->getInt64Value(chaos::NodeHealtDefinitionKey::NODE_HEALT_PROCESS_SWAP)));
    }
}

void HealtPresenterWidget::statusNoData(const QString& node_uid) {
    ui->labelUsrProc->setText(tr("---"));
    ui->labelSysProc->setText(tr("---"));
    ui->labelSwapProc->setText(tr("---"));
}
