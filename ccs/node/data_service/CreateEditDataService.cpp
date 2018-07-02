#include "CreateEditDataService.h"
#include "ui_CreateEditDataService.h"
#include <QtGui/QIntValidator>
CreateEditDataService::CreateEditDataService() :
    QDialog(NULL),
    current_row(-1),
    ui(new Ui::CreateEditDataService)
{
    ui->setupUi(this);
    ui->lineEditEndpoint->setValidator(new QIntValidator(0,255, this));
    connect(ui->buttonBox, SIGNAL(accepted()),
            this, SLOT(endWork()));
    setModal(true);

    ui->lineEditDirectIOAddress->setText(QString::fromStdString("localhost:1672:30175"));
}

CreateEditDataService::CreateEditDataService(int _current_row,
                                             const QString &unique_id,
                                             const QString &direct_io_address,
                                             int direct_io_endpoint):
    QDialog(NULL),
    current_row(_current_row),
    ui(new Ui::CreateEditDataService)
{
    ui->setupUi(this);
    ui->lineEditEndpoint->setValidator(new QIntValidator(0,255, this));
    connect(ui->buttonBox, SIGNAL(accepted()),
            this, SLOT(endWork()));
    setModal(true);
    ui->lineEditUniqueIdentifier->setText(unique_id);
    ui->lineEditUniqueIdentifier->setEnabled(false);
    ui->lineEditDirectIOAddress->setText(direct_io_address);
    ui->lineEditEndpoint->setText(QString::number(direct_io_endpoint));
}

CreateEditDataService::~CreateEditDataService()
{
    delete ui;
}

void CreateEditDataService::endWork() {
    const QString  uid = ui->lineEditUniqueIdentifier->text();
    const QString  direct_io_addr =  ui->lineEditDirectIOAddress->text();
    int endpoint = ui->lineEditEndpoint->text().toInt();

    if(current_row > -1) {
        //we are updating a driver description so emit the rigth signal
        emit updateDataService(current_row,
                               uid,
                               direct_io_addr,
                               endpoint);
    } else {
        //emit the value of the driver because user has accepted
        emit newDataService(uid,
                            direct_io_addr,
                            endpoint);
    }
}
