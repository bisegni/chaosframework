#include "AddNewDomain.h"
#include "ui_AddNewDomain.h"

AddNewDomain::AddNewDomain(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddNewDomain) {
    ui->setupUi(this);
}

AddNewDomain::~AddNewDomain() {
    delete ui;
}

QString AddNewDomain::getDomainName() {
    return ui->lineEditDomainName->text();
}

QString AddNewDomain::getRootName() {
     return ui->lineEditRootName->text();
}

void AddNewDomain::on_pushButton_clicked() {
    done(1);
}

void AddNewDomain::on_pushButton_2_clicked() {
    done(0);
}
