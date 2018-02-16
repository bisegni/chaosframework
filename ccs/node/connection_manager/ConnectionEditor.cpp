#include "ConnectionEditor.h"
#include "ui_ConnectionEditor.h"
#include "model/ControlUnitNodeDataModel.h"

#include <QUrl>
#include <QMimeData>
#include <QDropEvent>
#include <QDragEnterEvent>


using namespace QtNodes;
using namespace chaos::common::data::structured;
using namespace chaos::metadata_service_client::api_proxy::control_unit;

ConnectionEditor::ConnectionEditor(QWidget *parent) :
    QMainWindow(parent),
    api_submitter(this),
    scene(registerDataModels()),
    ui(new Ui::ConnectionEditor){
    setAcceptDrops(true);
    setWindowTitle("Connection Editor");
    ui->setupUi(this);
    ui->graphicsViewNodeEditor->setScene(&scene);
    ui->graphicsViewNodeEditor->setAcceptDrops(false);
}

ConnectionEditor::~ConnectionEditor() {
    delete ui;
}

std::shared_ptr<DataModelRegistry> ConnectionEditor::registerDataModels() {
    auto ret = std::make_shared<DataModelRegistry>();

    //ret->registerModel<ControlUnitNodeDataModel>();

    /*
         We could have more models registered.
         All of them become items in the context meny of the scene.

         ret->registerModel<AnotherDataModel>();
         ret->registerModel<OneMoreDataModel>();

       */

    return ret;
}

void ConnectionEditor::on_actionControl_Unit_triggered() {
    //    api_submitter.submitApiResult("load_dataset",
    //                                  GET_CHAOS_API_PTR(chaos::metadata_service_client::api_proxy::control_unit::GetCurrentDataset)->execute("control_unit_unique_id.toStdString()"));
}

void ConnectionEditor::on_actionSave_triggered() {
    scene.save();
}

void ConnectionEditor::onApiDone(const QString& tag,
                                 QSharedPointer<chaos::common::data::CDataWrapper> api_result) {
    qDebug() << "Add control unit " << tag;
    if(api_result.isNull()) return;

    std::unique_ptr<DatasetAttributeList> dataset_attribute_list(new DatasetAttributeList());
    GetCurrentDataset::deserialize(*api_result,
                                   *dataset_attribute_list);
    std::unique_ptr<ControlUnitNodeDataModel> new_node_mode(new ControlUnitNodeDataModel(tag,
                                                                                         dataset_attribute_list));
    scene.createNode(std::move(new_node_mode));
}

void ConnectionEditor::dragEnterEvent(QDragEnterEvent *e) {
    const QStringList fl = e->mimeData()->formats();
    qDebug() << "format:" << fl;
    if (fl.contains("application/chaos-node-uid-type-list")) {
        e->acceptProposedAction();
    }
}

void ConnectionEditor::dragMoveEvent(QDragMoveEvent* e) {
    e->acceptProposedAction();
}

void ConnectionEditor::dragLeaveEvent(QDragLeaveEvent* e) {
    e->accept();
}

void ConnectionEditor::dropEvent(QDropEvent *e) {
    qDebug() << "drop" << e->mimeData()->formats();
    const QMimeData* data = e->mimeData();
    if (!data->hasFormat("application/chaos-node-uid-type-list"))
        return;
    QByteArray encodedData = data->data("application/chaos-node-uid-type-list");
    QDataStream stream(&encodedData, QIODevice::ReadOnly);

    while (!stream.atEnd()) {
        QString node_uid;
        QString node_type;
        stream >> node_uid;
        stream >> node_type;

        if(node_type.compare(chaos::NodeType::NODE_TYPE_CONTROL_UNIT) == 0) {
            qDebug() << "Load dataset for control unit " << node_uid;
            api_submitter.submitApiResult(node_uid,
                                          GET_CHAOS_API_PTR(GetCurrentDataset)->execute(node_uid.toStdString()));
        }
    }
}

