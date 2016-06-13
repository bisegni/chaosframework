#include "ExecutionPoolsManager.h"
#include "ui_ExecutionPoolsManager.h"

ExecutionPoolsManager::ExecutionPoolsManager(QWidget *parent) :
    PresenterWidget(parent),
    ui(new Ui::ExecutionPoolsManager) {
    ui->setupUi(this);
}

ExecutionPoolsManager::~ExecutionPoolsManager() {
    delete ui;
}

bool ExecutionPoolsManager::isClosing() {

}

void ExecutionPoolsManager::initUI() {

}

void ExecutionPoolsManager::onApiDone(const QString& tag,
                                      QSharedPointer<chaos::common::data::CDataWrapper> api_result) {

}
