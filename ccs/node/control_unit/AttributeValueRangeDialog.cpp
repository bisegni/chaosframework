#include "AttributeValueRangeDialog.h"
#include "ui_AttributeValueRangeDialog.h"


AttributeValueRangeDialog::AttributeValueRangeDialog(QWidget *parent) :
    QDialog(parent),
    current_row(-1),
    attribute_name(QString("")),
    attribute_default_value(QString("")),
    attribute_default_max_value(QString("")),
    attribute_default_min_value(QString("")),
    ui(new Ui::AttributeValueRangeDialog)
{
    ui->setupUi(this);
    connect(ui->buttonBox, SIGNAL(accepted()),
            this, SLOT(endWork()));
    setModal(true);
}

AttributeValueRangeDialog::AttributeValueRangeDialog(int _current_row,
                                                     const QString& _attribute_name,
                                                     const QString& _attribute_default_value,
                                                     const QString& _attribute_default_max_value,
                                                     const QString& _attribute_default_min_value,
                                                     QWidget *parent):
    QDialog(parent),
    current_row(_current_row),
    attribute_name(_attribute_name),
    attribute_default_value(_attribute_default_value),
    attribute_default_max_value(_attribute_default_max_value),
    attribute_default_min_value(_attribute_default_min_value),
    ui(new Ui::AttributeValueRangeDialog)
{
    ui->setupUi(this);
    connect(ui->buttonBox, SIGNAL(accepted()),
            this, SLOT(endWork()));
    setModal(true);

    //set the value for the editing
    ui->lineEditAttributeName->setText(_attribute_name);
    ui->lineEditDefaultValue->setText(_attribute_default_value);
    ui->lineEditMaxValue->setText(_attribute_default_max_value);
    ui->lineEditMinValue->setText(_attribute_default_min_value);
}

AttributeValueRangeDialog::~AttributeValueRangeDialog()
{
    delete ui;
}

void AttributeValueRangeDialog::endWork() {
    if(current_row > -1) {
        //we are updating a driver description so emit the rigth signal
        emit updateAttributeDescription(current_row,
                                        ui->lineEditAttributeName->text(),
                                        ui->lineEditDefaultValue->text(),
                                        ui->lineEditMaxValue->text(),
                                        ui->lineEditMinValue->text());
    } else {
        //emit the value of the driver because user has accepted
        emit newAttributeDescription(ui->lineEditAttributeName->text(),
                                     ui->lineEditDefaultValue->text(),
                                     ui->lineEditMaxValue->text(),
                                     ui->lineEditMinValue->text());
    }
}
