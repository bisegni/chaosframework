#include "ControUnitInstanceEditor.h"
#include "ui_ControUnitInstanceEditor.h"

using namespace chaos::common::data;
ControUnitInstanceEditor::ControUnitInstanceEditor(const QString& unit_server_uid) :
    PresenterWidget(NULL),
    unit_server_uid(unit_server_uid),
    control_unit_type(""),
    control_unit_uid(""),
    ui(new Ui::ControUnitInstanceEditor)
{
    ui->setupUi(this);
}

ControUnitInstanceEditor::ControUnitInstanceEditor(const QString& unit_server_uid,
                                                   const QString& control_unit_type):
    PresenterWidget(NULL),
    unit_server_uid(unit_server_uid),
    control_unit_type(control_unit_type),
    control_unit_uid(""),
    ui(new Ui::ControUnitInstanceEditor)
{
    ui->setupUi(this);
}

ControUnitInstanceEditor::ControUnitInstanceEditor(const QString& unit_server_uid,
                                                   const QString& control_unit_type,
                                                   const QString& control_unit_uid):
    PresenterWidget(NULL),
    unit_server_uid(unit_server_uid),
    control_unit_type(control_unit_type),
    control_unit_uid(control_unit_uid),
    ui(new Ui::ControUnitInstanceEditor)
{
    ui->setupUi(this);
}

ControUnitInstanceEditor::~ControUnitInstanceEditor()
{
    delete ui;
}

void ControUnitInstanceEditor::initUI() {

    //initilization view
    is_in_editing = unit_server_uid.size() &&
            control_unit_type.size() &&
            control_unit_uid.size();

    //setting the default info
    ui->labelUnitServer->setText(unit_server_uid);
    ui->labelControlUnitType->setText(control_unit_type);
    if(is_in_editing) {
        ui->lineEditControlUnitUniqueID->setText(control_unit_uid);
        setTabTitle(tr("Control unit instance editing"));
    }else{
        setTabTitle(tr("Control unit instance creation"));
    }

    // configure driver table
    table_model_driver_spec = new QStandardItemModel(this);
    table_model_driver_spec->setHorizontalHeaderItem(0, new QStandardItem(QString("Name")));
    table_model_driver_spec->setHorizontalHeaderItem(1, new QStandardItem(QString("Version")));
    table_model_driver_spec->setHorizontalHeaderItem(2, new QStandardItem(QString("Init parameter")));

    // Attach the model to the view
    ui->tableViewDriverSpecification->setModel(table_model_driver_spec);

    QHeaderView *headerView = ui->tableViewDriverSpecification->horizontalHeader();
    headerView->setSectionResizeMode(QHeaderView::Stretch);

    //finisch to configure table
    ui->tableViewDriverSpecification->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableViewDriverSpecification->setSelectionBehavior(QAbstractItemView::SelectRows);

    //configure dataset attribute table

    table_model_dataset_attribute_setup = new QStandardItemModel(this);
    table_model_dataset_attribute_setup->setHorizontalHeaderItem(0, new QStandardItem(QString("Name")));
    table_model_dataset_attribute_setup->setHorizontalHeaderItem(1, new QStandardItem(QString("Max Value")));
    table_model_dataset_attribute_setup->setHorizontalHeaderItem(2, new QStandardItem(QString("Min Value")));
    table_model_dataset_attribute_setup->setHorizontalHeaderItem(3, new QStandardItem(QString("Default Value")));

    // Attach the model to the view
    ui->tableViewDatasetAttributes->setModel(table_model_dataset_attribute_setup);

    headerView = ui->tableViewDatasetAttributes->horizontalHeader();
    headerView->setSectionResizeMode(QHeaderView::Stretch);

    //finisch to configure table
    ui->tableViewDatasetAttributes->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableViewDatasetAttributes->setSelectionBehavior(QAbstractItemView::SelectRows);
}

bool ControUnitInstanceEditor::canClose() {
    return true;
}

void ControUnitInstanceEditor::onApiDone(QString tag,
                                         QSharedPointer<CDataWrapper> api_result) {

}
