#include "CreateNewScriptDialog.h"
#include "ui_CreateNewScriptDialog.h"

using namespace chaos::service_common::data::script;

CreateNewScriptDialog::CreateNewScriptDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CreateNewScriptDialog)
{
    ui->setupUi(this);
}

CreateNewScriptDialog::~CreateNewScriptDialog() {
    delete ui;
}

void CreateNewScriptDialog::fillScript(Script& script_description){
    script_description.script_description.name = ui->lineEdit->text().toStdString();
    script_description.script_description.description = ui->plainTextEdit->document()->toPlainText().toStdString();
}

void CreateNewScriptDialog::on_pushButtonCreateScript_clicked() {
    accept();
}

void CreateNewScriptDialog::on_pushButtonCancel_clicked() {
    reject();
}
