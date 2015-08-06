#include "DriverDescriptionInputDialog.h"
#include "ui_DriverDescriptionInputDialog.h"

DriverDescriptionInputDialog::DriverDescriptionInputDialog(QWidget *_parent) :
    QDialog(_parent),
    current_row(-1),
    driver_name(QString("")),
    driver_version(QString("")),
    driver_init_parameter(QString("")),
    ui(new Ui::DriverDescriptionInputDialog)
{
    ui->setupUi(this);
    connect(ui->buttonBox, SIGNAL(accepted()),
            this, SLOT(endWork()));
    setModal(true);
}

DriverDescriptionInputDialog::DriverDescriptionInputDialog(int _current_row,
                                                           const QString& _driver_name,
                                                           const QString& _driver_version,
                                                           const QString& _driver_init_parameter,
                                                           QWidget *_parent):
    QDialog(_parent),
    current_row(_current_row),
    driver_name(_driver_name),
    driver_version(_driver_version),
    driver_init_parameter(_driver_init_parameter),
    ui(new Ui::DriverDescriptionInputDialog)
{
    ui->setupUi(this);
    connect(ui->buttonBox, SIGNAL(accepted()),
            this, SLOT(endWork()));
    setModal(true);

    //set the value for the editing
    ui->lineEditName->setText(driver_name);
    ui->lineEditVersion->setText(driver_version);
    ui->textEditInitParameter->setText(driver_init_parameter);
}

DriverDescriptionInputDialog::~DriverDescriptionInputDialog()
{
    delete ui;
}

void DriverDescriptionInputDialog::endWork() {
    if(current_row > -1) {
        //we are updating a driver description so emit the rigth signal
        emit updateDriverDescription(current_row,
                                     ui->lineEditName->text(),
                                     ui->lineEditVersion->text(),
                                     ui->textEditInitParameter->toPlainText());
    } else {
        //emit the value of the driver because user has accepted
        emit newDriverDescription(ui->lineEditName->text(),
                                  ui->lineEditVersion->text(),
                                  ui->textEditInitParameter->toPlainText());
    }
}
