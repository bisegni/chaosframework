#include "ScriptDescriptionWidget.h"
#include "ui_ScriptDescriptionWidget.h"

ScriptDescriptionWidget::ScriptDescriptionWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ScriptDescriptionWidget) {
    ui->setupUi(this);
}

ScriptDescriptionWidget::~ScriptDescriptionWidget() {
    delete ui;
}

void ScriptDescriptionWidget::on_lineEditScriptName_editingFinished() {

}
