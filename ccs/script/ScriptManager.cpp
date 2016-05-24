#include "ScriptManager.h"
#include "ui_ScriptManager.h"

ScriptManager::ScriptManager(QWidget *parent) :
    PresenterWidget(parent),
    ui(new Ui::ScriptManager) {
    ui->setupUi(this);
    QList<int> sizes;
    sizes << (size().width()*1/5) << (size().width()*4/5);
    ui->splitterMain->setSizes(sizes);
}

ScriptManager::~ScriptManager() {
    delete ui;
}

bool ScriptManager::isClosing() {
    return true;
}

void ScriptManager::initUI() {

}

void ScriptManager::on_pushButtonCreateNewScript_clicked() {

}

void ScriptManager::on_pushButtonDeleteScript_clicked() {

}
