#include "ScriptManager.h"
#include "ui_ScriptManager.h"

#include <QDebug>

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


void ScriptManager::initUI() {
    ui->listViewScriptList->setModel(&script_list_model);
}

bool ScriptManager::isClosing() {
    return true;
}


void ScriptManager::on_pushButtonCreateNewScript_clicked() {

}

void ScriptManager::on_pushButtonDeleteScript_clicked() {

}


void ScriptManager::on_pushButtonStartSearch_clicked() {
    script_list_model.updateSearchString(ui->lineEditSearchString->text());

}
