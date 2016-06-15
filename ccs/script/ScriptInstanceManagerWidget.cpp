#include "ScriptInstanceManagerWidget.h"
#include "ui_ScriptInstanceManagerWidget.h"

ScriptInstanceManagerWidget::ScriptInstanceManagerWidget(QWidget *parent) :
    QWidget(parent),
    api_submitter(this),
    ui(new Ui::ScriptInstanceManagerWidget) {
    ui->setupUi(this);
}

ScriptInstanceManagerWidget::~ScriptInstanceManagerWidget() {
    delete ui;
}

void ScriptInstanceManagerWidget::onApiDone(const QString& tag,
                                            QSharedPointer<chaos::common::data::CDataWrapper> api_result) {

}

void ScriptInstanceManagerWidget::on_pushButtonSearchInstances_clicked() {

}

void ScriptInstanceManagerWidget::on_pushButtonAddNew_clicked() {

}

void ScriptInstanceManagerWidget::on_pushButtonremoveInstance_clicked() {

}
