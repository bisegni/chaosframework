#include "ConnectionEditor.h"
#include "ui_ConnectionEditor.h"
#include "model/ControlUnitNodeDataModel.h"
using namespace QtNodes;

ConnectionEditor::ConnectionEditor(QWidget *parent) :
    QMainWindow(parent),
    scene(registerDataModels()),
    ui(new Ui::ConnectionEditor){
    setWindowTitle("Connection Editor");
    ui->setupUi(this);
    ui->graphicsViewNodeEditor->setScene(&scene);
}

ConnectionEditor::~ConnectionEditor() {
    delete ui;
}

std::shared_ptr<DataModelRegistry> ConnectionEditor::registerDataModels() {
    auto ret = std::make_shared<DataModelRegistry>();

    ret->registerModel<ControlUnitNodeDataModel>();

    /*
         We could have more models registered.
         All of them become items in the context meny of the scene.

         ret->registerModel<AnotherDataModel>();
         ret->registerModel<OneMoreDataModel>();

       */

    return ret;

}

void ConnectionEditor::on_actionControl_Unit_triggered() {
    std::unique_ptr<ControlUnitNodeDataModel> new_node_mode(new ControlUnitNodeDataModel());
    scene.createNode(std::move(new_node_mode));
}
