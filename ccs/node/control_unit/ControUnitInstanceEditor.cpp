#include "ControUnitInstanceEditor.h"
#include "ui_ControUnitInstanceEditor.h"
#include "DriverDescriptionInputDialog.h"
#include "AttributeValueRangeDialog.h"

#include <QDebug>
#include <QStandardItem>
#include <QMessageBox>
#include <QInputDialog>
#include <QIntValidator>
#include <cassert>

using namespace chaos::common::data;
using namespace chaos::metadata_service_client;
using namespace chaos::metadata_service_client::api_proxy;

ControUnitInstanceEditor::ControUnitInstanceEditor(const QString& unit_server_uid,
                                                   const QString& control_unit_type):
    PresenterWidget(NULL),
    script_enditor(NULL),
    is_in_editing(false),
    ui(new Ui::ControUnitInstanceEditor) {
    ui->setupUi(this);
    ui->labelUnitServerUID->setText(unit_server_uid);
    ui->labelControlUnitType->setText(control_unit_type);
}

ControUnitInstanceEditor::ControUnitInstanceEditor(const QString& unit_server_uid,
                                                   const QString& control_unit_uid,
                                                   bool edit_instance):
    PresenterWidget(NULL),
    script_enditor(NULL),
    is_in_editing(edit_instance),
    ui(new Ui::ControUnitInstanceEditor) {
    ui->setupUi(this);
    ui->labelUnitServerUID->setText(unit_server_uid);
    ui->lineEditControlUnitUniqueID->setText(control_unit_uid);
    ui->lineEditControlUnitUniqueID->setEnabled(false);
}

ControUnitInstanceEditor::~ControUnitInstanceEditor() {
    delete ui;
}

void ControUnitInstanceEditor::initUI() {
    //setting the default info
    if(is_in_editing) {
        setTitle(tr("Control unit instance editing"));
    }else{
        setTitle(tr("Control unit instance creation"));
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

    //configure dataset attribute table

    table_model_dataset_attribute_setup = new QStandardItemModel(this);
    table_model_dataset_attribute_setup->setHorizontalHeaderItem(0, new QStandardItem(QString("Name")));
    table_model_dataset_attribute_setup->setHorizontalHeaderItem(1, new QStandardItem(QString("Default Value")));
    table_model_dataset_attribute_setup->setHorizontalHeaderItem(2, new QStandardItem(QString("Max Value")));
    table_model_dataset_attribute_setup->setHorizontalHeaderItem(3, new QStandardItem(QString("Min Value")));

    // Attach the model to the view
    ui->tableViewDatasetAttributes->setModel(table_model_dataset_attribute_setup);

    headerView = ui->tableViewDatasetAttributes->horizontalHeader();
    headerView->setSectionResizeMode(QHeaderView::Stretch);

    //set the run schedule dealy data formatted for lineedit(from 0 to one hour of delay)
    ui->lineEditDefaultScheduleTime->setValidator(new QIntValidator(0, 1000*1000*60, this));

    //widget connection initialization
    connect(ui->tableViewDriverSpecification->selectionModel(),
            SIGNAL(currentChanged(QModelIndex,QModelIndex)),
            SLOT(tableDriverDescriptionCurrentChanged(QModelIndex,QModelIndex)));

    connect(ui->tableViewDriverSpecification->selectionModel(),
            SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
            SLOT(tableDriverDescriptionSelectionChanged(QItemSelection,QItemSelection)));

    connect(ui->tableViewDatasetAttributes->selectionModel(),
            SIGNAL(currentChanged(QModelIndex,QModelIndex)),
            SLOT(tableDriverDescriptionCurrentChanged(QModelIndex,QModelIndex)));

    connect(ui->tableViewDatasetAttributes->selectionModel(),
            SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
            SLOT(tableDriverDescriptionSelectionChanged(QItemSelection,QItemSelection)));

    if(is_in_editing) {
        //get information from server
        submitApiResult(QString("get_instance"),
                        GET_CHAOS_API_PTR(control_unit::GetInstance)->execute(ui->lineEditControlUnitUniqueID->text().toStdString()));
    }

    //get unit server informationi
    if(ui->labelUnitServerUID->text().size() > 0) {
        submitApiResult(QString("get_us_description"),
                        GET_CHAOS_API_PTR(unit_server::GetDescription)->execute(ui->labelUnitServerUID->text().toStdString()));
    }
    //set limit for ageing field
    ui->lineEditHistoryAgeing->setValidator(new QIntValidator(0, std::numeric_limits<int>::max(), this));
    ui->lineEditHistoryTime->setValidator(new QIntValidator(0, std::numeric_limits<int>::max(), this));
    ui->lineEditLiveTime->setValidator(new QIntValidator(0, std::numeric_limits<int>::max(), this));
}

control_unit::SetInstanceDescriptionHelper& ControUnitInstanceEditor::prepareSetInstanceApi() {
    //cu id
    set_instance_api_hepler.control_unit_uid = ui->lineEditControlUnitUniqueID->text().toStdString();
    //us id
    set_instance_api_hepler.unit_server_uid = ui->labelUnitServerUID->text().toStdString();
    //cu implementation
    set_instance_api_hepler.control_unit_implementation = ui->labelControlUnitType->text().toStdString();
    //autoload
    set_instance_api_hepler.auto_load = ui->checkBoxAutoLoad->isChecked();
    //autoload
    set_instance_api_hepler.auto_init = ui->checkBoxAutoInit->isChecked();
    //autoload
    set_instance_api_hepler.auto_start = ui->checkBoxAutoStart->isChecked();
    //load parameter
    set_instance_api_hepler.load_parameter = ui->textEditLoadParameter->toPlainText().toStdString();
    //schedule delay
    set_instance_api_hepler.default_schedule_delay = ui->lineEditDefaultScheduleTime->text().toULongLong();

    //storage configuration
    set_instance_api_hepler.storage_type = static_cast<chaos::DataServiceNodeDefinitionType::DSStorageType>(ui->comboBoxStorageType->currentIndex());
    set_instance_api_hepler.history_ageing = ui->lineEditHistoryAgeing->text().toUInt();
    set_instance_api_hepler.history_time = ui->lineEditHistoryTime->text().toULongLong();
    set_instance_api_hepler.live_time = ui->lineEditLiveTime->text().toULongLong();
    //add all driver description
    set_instance_api_hepler.clearAllDriverDescriptions();
    for(int idx = 0;
        idx < table_model_driver_spec->rowCount();
        idx++) {
        QString drv_name = table_model_driver_spec->item(idx, 0)->text();
        QString drv_vers = table_model_driver_spec->item(idx, 1)->text();
        QString drv_init = table_model_driver_spec->item(idx, 2)->text();
        set_instance_api_hepler.addDriverDescription(drv_name.toStdString(),
                                                     drv_vers.toStdString(),
                                                     drv_init.toStdString());
        qDebug() << "Added driver description for: "<< drv_name << "-" << drv_vers <<"-"<<drv_init;
    }

    //add all attribute description
    set_instance_api_hepler.clearAllAttributeConfig();
    for(int idx = 0;
        idx < table_model_dataset_attribute_setup->rowCount();
        idx++) {
        QString attr_name = table_model_dataset_attribute_setup->item(idx, 0)->text();
        QString attr_def = table_model_dataset_attribute_setup->item(idx, 1)->text();
        QString attr_max = table_model_dataset_attribute_setup->item(idx, 2)->text();
        QString attr_min = table_model_dataset_attribute_setup->item(idx, 3)->text();
        set_instance_api_hepler.addAttributeConfig(attr_name.toStdString(),
                                                   attr_def.toStdString(),
                                                   attr_max.toStdString(),
                                                   attr_min.toStdString());
        qDebug() << "Added attribute description for: "<< attr_name <<"-" << attr_def << " of ["<<attr_min<<"/"<<attr_max<<"]";
    }
    return set_instance_api_hepler;
}

#define CHECK_AND_SET_LABEL(x,v)  if(api_result->hasKey(x)) {  v->setText(QString::fromStdString(api_result->getStringValue(x))); } else {  v->setText(QString());}
#define CHECK_AND_SET_CHECK(x,v) if(api_result->hasKey(x)) { v->setChecked(api_result->getBoolValue(x)); } else {  v->setChecked(false);}

void ControUnitInstanceEditor::fillUIFromInstanceInfo(QSharedPointer<chaos::common::data::CDataWrapper> api_result) {
    table_model_driver_spec->setRowCount(0);
    table_model_dataset_attribute_setup->setRowCount(0);
    if(api_result->hasKey("control_unit_implementation") &&
            api_result->isStringValue("control_unit_implementation")) {
        ui->textEditLoadParameter->setEnabled(api_result->getStringValue("control_unit_implementation").compare("ScriptableExecutionUnit") != 0);
    }
    CHECK_AND_SET_LABEL(chaos::NodeDefinitionKey::NODE_PARENT, ui->labelUnitServerUID)
            CHECK_AND_SET_LABEL(chaos::NodeDefinitionKey::NODE_UNIQUE_ID, ui->lineEditControlUnitUniqueID)
            CHECK_AND_SET_LABEL("control_unit_implementation", ui->labelControlUnitType)
            CHECK_AND_SET_CHECK("auto_load", ui->checkBoxAutoLoad)
            CHECK_AND_SET_CHECK("auto_init", ui->checkBoxAutoInit)
            CHECK_AND_SET_CHECK("auto_start", ui->checkBoxAutoStart)
            CHECK_AND_SET_LABEL(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_LOAD_PARAM, ui->textEditLoadParameter)
    if(api_result->hasKey(chaos::ControlUnitDatapackSystemKey::THREAD_SCHEDULE_DELAY)){
        ui->lineEditDefaultScheduleTime->setText(QString::number(api_result->getUInt64Value(chaos::ControlUnitDatapackSystemKey::THREAD_SCHEDULE_DELAY)));
    }
    if(api_result->hasKey(chaos::DataServiceNodeDefinitionKey::DS_STORAGE_TYPE)){
        ui->comboBoxStorageType->setCurrentIndex(api_result->getUInt32Value(chaos::DataServiceNodeDefinitionKey::DS_STORAGE_TYPE));
    }
    if(api_result->hasKey(chaos::DataServiceNodeDefinitionKey::DS_STORAGE_HISTORY_AGEING)){
        ui->lineEditHistoryAgeing->setText(QString::number(api_result->getUInt32Value(chaos::DataServiceNodeDefinitionKey::DS_STORAGE_HISTORY_AGEING)));
    }
    if(api_result->hasKey(chaos::DataServiceNodeDefinitionKey::DS_STORAGE_HISTORY_TIME)){
        ui->lineEditHistoryTime->setText(QString::number(api_result->getUInt64Value(chaos::DataServiceNodeDefinitionKey::DS_STORAGE_HISTORY_TIME)));
    }
    if(api_result->hasKey(chaos::DataServiceNodeDefinitionKey::DS_STORAGE_LIVE_TIME)){
        ui->lineEditLiveTime->setText(QString::number(api_result->getUInt64Value(chaos::DataServiceNodeDefinitionKey::DS_STORAGE_LIVE_TIME)));
    }
    //add driverdesc
    if(api_result->hasKey(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DRIVER_DESCRIPTION)) {
        std::auto_ptr<CMultiTypeDataArrayWrapper> arr_drv(api_result->getVectorValue(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DRIVER_DESCRIPTION));
        for(int idx = 0;
            idx != arr_drv->size();
            idx++) {
            std::auto_ptr<CDataWrapper> drv_desc(arr_drv->getCDataWrapperElementAtIndex(idx));

            QStandardItem *item = NULL;
            QList<QStandardItem*> row_item;
            row_item.append(item = new QStandardItem(QString::fromStdString(drv_desc->getStringValue(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DRIVER_DESCRIPTION_NAME))));
            item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);

            row_item.append(item = new QStandardItem(QString::fromStdString(drv_desc->getStringValue(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DRIVER_DESCRIPTION_VERSION))));
            item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);

            row_item.append(item = new QStandardItem(QString::fromStdString(drv_desc->getStringValue(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DRIVER_DESCRIPTION_INIT_PARAMETER))));
            item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
            table_model_driver_spec->appendRow(row_item);

            ui->pushButtonEditDriverDescription->setEnabled(false);
        }
    }
    //add attribute desc
    if(api_result->hasKey("attribute_value_descriptions")) {
        std::auto_ptr<CMultiTypeDataArrayWrapper> arr_attr(api_result->getVectorValue("attribute_value_descriptions"));
        for(int idx = 0;
            idx != arr_attr->size();
            idx++) {
            std::auto_ptr<CDataWrapper> attr_desc(arr_attr->getCDataWrapperElementAtIndex(idx));

            QStandardItem *item = NULL;
            QList<QStandardItem*> row_item;
            if(attr_desc->hasKey(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_NAME)) {
                row_item.append(item = new QStandardItem(QString::fromStdString(attr_desc->getStringValue(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_NAME))));
            } else {
                row_item.append(item = new QStandardItem(QString()));
            }
            item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);

            if(attr_desc->hasKey(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_DEFAULT_VALUE)) {
                row_item.append(item = new QStandardItem(QString::fromStdString(attr_desc->getStringValue(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_DEFAULT_VALUE))));
            } else {
                row_item.append(item = new QStandardItem(QString()));
            }
            item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);

            if(attr_desc->hasKey(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_MAX_RANGE)) {
                row_item.append(item = new QStandardItem(QString::fromStdString(attr_desc->getStringValue(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_MAX_RANGE))));
            } else {
                row_item.append(item = new QStandardItem(QString()));
            }
            item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);

            if(attr_desc->hasKey(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_MIN_RANGE)) {
                row_item.append(item = new QStandardItem(QString::fromStdString(attr_desc->getStringValue(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_MIN_RANGE))));
            } else {
                row_item.append(item = new QStandardItem(QString()));
            }
            item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
            table_model_dataset_attribute_setup->appendRow(row_item);

            ui->pushButtonEditAttribute->setEnabled(false);
        }
    }
}

bool ControUnitInstanceEditor::isClosing() {
    //in case we have editor open we close it
    cancelScriptEditing();
    return true;
}

void ControUnitInstanceEditor::onApiDone(const QString& tag,
                                         QSharedPointer<CDataWrapper> api_result) {
    if(tag.compare("save_instance") == 0) {
        QMessageBox::information(this,
                                 tr("Instance Save"),
                                 tr("Instance has been successfull saved"));
        //close editor
        //closeTab();
    } else if(tag.compare("get_instance") == 0) {
        //fill gui with instance info
        fillUIFromInstanceInfo(api_result);
    } else if(tag.compare("get_us_description") == 0) {
        //we have unit server description
        if(api_result->hasKey(chaos::UnitServerNodeDefinitionKey::UNIT_SERVER_HOSTED_CONTROL_UNIT_CLASS)) {
            //get the vector of unit type
            std::auto_ptr<CMultiTypeDataArrayWrapper> arr(api_result->getVectorValue(chaos::UnitServerNodeDefinitionKey::UNIT_SERVER_HOSTED_CONTROL_UNIT_CLASS));
            for(int i = 0;
                i < arr->size();
                i++) {
                cu_type_list.append(QString::fromStdString(arr->getStringElementAtIndex(i)));
            }
        }
    }
    PresenterWidget::onApiDone(tag,
                               api_result);
}

void ControUnitInstanceEditor::on_pushButtonSaveInstance_clicked() {
    submitApiResult(QString("save_instance"),
                    GET_CHAOS_API_PTR(control_unit::SetInstanceDescription)->execute(prepareSetInstanceApi()));
}

void ControUnitInstanceEditor::on_pushButtonAddDriverDescription_clicked()
{
    //add new driver description
    DriverDescriptionInputDialog new_driver_dialog(this);
    connect(&new_driver_dialog,
            SIGNAL(newDriverDescription(QString,QString,QString)),
            SLOT(addNewDriverDescription(QString,QString,QString)));
    new_driver_dialog.exec();
}

void ControUnitInstanceEditor::on_pushButtonEditDriverDescription_clicked()
{
    //mod driver description
    QModelIndex current_index = ui->tableViewDriverSpecification->selectionModel()->selectedRows().first();
    DriverDescriptionInputDialog edit_driver_dialog(current_index.row(),
                                                    table_model_driver_spec->item(current_index.row(), 0)->text(),
                                                    table_model_driver_spec->item(current_index.row(), 1)->text(),
                                                    table_model_driver_spec->item(current_index.row(), 2)->text(),
                                                    this);
    connect(&edit_driver_dialog,
            SIGNAL(updateDriverDescription(int,QString,QString,QString)),
            SLOT(updateDriverDescription(int,QString,QString,QString)));
    edit_driver_dialog.exec();
}

void ControUnitInstanceEditor::on_pushButtonDeleteDriverDescription_clicked()
{
    //delete driver description
    QModelIndexList selected_row = ui->tableViewDriverSpecification->selectionModel()->selectedRows();
    foreach(QModelIndex index, selected_row) {
        table_model_driver_spec->removeRow(index.row());
    }
}


void ControUnitInstanceEditor::on_pushButtonAddAttribute_clicked()
{
    //add new attribute description
    AttributeValueRangeDialog new_attr_dialog(this);
    connect(&new_attr_dialog,
            SIGNAL(newAttributeDescription(QString,QString,QString,QString)),
            SLOT(addAttributeDescription(QString,QString,QString,QString)));
    new_attr_dialog.exec();
}

void ControUnitInstanceEditor::on_pushButtonEditAttribute_clicked()
{
    //add new attribute description
    //mod driver description
    QModelIndex current_index = ui->tableViewDatasetAttributes->selectionModel()->selectedRows().first();
    AttributeValueRangeDialog edit_attr_dialog(current_index.row(),
                                               table_model_dataset_attribute_setup->item(current_index.row(), 0)->text(),
                                               table_model_dataset_attribute_setup->item(current_index.row(), 1)->text(),
                                               table_model_dataset_attribute_setup->item(current_index.row(), 2)->text(),
                                               table_model_dataset_attribute_setup->item(current_index.row(), 3)->text(),
                                               this);
    connect(&edit_attr_dialog,
            SIGNAL(updateAttributeDescription(int,QString,QString,QString,QString)),
            SLOT(updateAttributeDescription(int,QString,QString,QString,QString)));
    edit_attr_dialog.exec();
}

void ControUnitInstanceEditor::on_pushButtonRemoveAttribute_clicked()
{
    //delete driver description
    QModelIndexList selected_row = ui->tableViewDatasetAttributes->selectionModel()->selectedIndexes();
    foreach(QModelIndex index, selected_row) {
        table_model_dataset_attribute_setup->removeRow(index.row());
    }
}

void ControUnitInstanceEditor::addNewDriverDescription(const QString& driver_name,
                                                       const QString& driver_version,
                                                       const QString& driver_init_parameter) {

    QStandardItem *item = NULL;
    QList<QStandardItem*> row_item;
    row_item.append(item = new QStandardItem(driver_name));
    item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);

    row_item.append(item = new QStandardItem(driver_version));
    item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);

    row_item.append(item = new QStandardItem(driver_init_parameter));
    item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
    table_model_driver_spec->appendRow(row_item);
    ui->pushButtonEditDriverDescription->setEnabled(false);
}

void ControUnitInstanceEditor::updateDriverDescription(int current_row,
                                                       const QString& driver_name,
                                                       const QString& driver_version,
                                                       const QString& driver_init_parameter) {
    table_model_driver_spec->setItem(current_row, 0, new QStandardItem(driver_name));
    table_model_driver_spec->setItem(current_row, 1, new QStandardItem(driver_version));
    table_model_driver_spec->setItem(current_row, 2, new QStandardItem(driver_init_parameter));
    ui->pushButtonEditDriverDescription->setEnabled(false);
}

void ControUnitInstanceEditor::addAttributeDescription(const QString& attribute_name,
                                                       const QString& attribute_default_value,
                                                       const QString& attribute_default_max_value,
                                                       const QString& attribute_default_min_value) {
    QStandardItem *item = NULL;
    QList<QStandardItem*> row_item;
    row_item.append(item = new QStandardItem(attribute_name));
    item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);

    row_item.append(item = new QStandardItem(attribute_default_value));
    item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);

    row_item.append(item = new QStandardItem(attribute_default_max_value));
    item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);

    row_item.append(item = new QStandardItem(attribute_default_min_value));
    item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
    table_model_dataset_attribute_setup->appendRow(row_item);
    ui->pushButtonEditAttribute->setEnabled(false);
}

void ControUnitInstanceEditor::updateAttributeDescription(int current_row,
                                                          const QString& attribute_name,
                                                          const QString& attribute_default_value,
                                                          const QString& attribute_default_max_value,
                                                          const QString& attribute_default_min_value) {
    table_model_dataset_attribute_setup->setItem(current_row, 0, new QStandardItem(attribute_name));
    table_model_dataset_attribute_setup->setItem(current_row, 1, new QStandardItem(attribute_default_value));
    table_model_dataset_attribute_setup->setItem(current_row, 2, new QStandardItem(attribute_default_max_value));
    table_model_dataset_attribute_setup->setItem(current_row, 3, new QStandardItem(attribute_default_min_value));
    ui->pushButtonEditAttribute->setEnabled(false);
}

void ControUnitInstanceEditor::tableDriverDescriptionCurrentChanged(const QModelIndex &current,
                                                                    const QModelIndex &previous) {
    (void)current;
    (void)previous;
    //enable the edit button only if is selected one element on table
    bool enable = false;
    if(QObject::sender() == ui->tableViewDriverSpecification) {
        enable = ui->tableViewDriverSpecification->selectionModel()->selectedRows().size()==1;
        ui->pushButtonEditDriverDescription->setEnabled(enable);
    }else{
        enable = ui->tableViewDatasetAttributes->selectionModel()->selectedRows().size()==1;
        ui->pushButtonEditAttribute->setEnabled(enable);
    }
    qDebug() << "tableDriverDescriptionCurrentChanged, edit enable state:" <<enable;
}

void ControUnitInstanceEditor::tableDriverDescriptionSelectionChanged(const QItemSelection& selected,
                                                                      const QItemSelection& unselected) {
    (void)selected;
    (void)unselected;
    //enable the edit button only if is selected one element on table
    bool enable = false;
    if(QObject::sender() == ui->tableViewDriverSpecification->selectionModel()) {
        enable = ui->tableViewDriverSpecification->selectionModel()->selectedRows().size()==1;
        ui->pushButtonEditDriverDescription->setEnabled(enable);
    }else{
        enable = ui->tableViewDatasetAttributes->selectionModel()->selectedRows().size()==1;
        ui->pushButtonEditAttribute->setEnabled(enable);
    }
    qDebug() << "tableDriverDescriptionSelectionChanged, edit enable state:" << enable;
}


void ControUnitInstanceEditor::on_pushButtonChooseControlUnitType_clicked() {
    bool ok = false;
    QString type = QInputDialog::getItem(this,
                                         tr("Select control unit type"),
                                         tr("Type:"),
                                         cu_type_list,
                                         0,
                                         false,
                                         &ok);
    if(ok) {
        ui->labelControlUnitType->setText(type);
    }
}

void ControUnitInstanceEditor::on_pushButton_clicked() {
    //edit load parameter in script editor
    if(script_enditor == NULL) {
        script_enditor = new ScriptEditor();
        connect(script_enditor, SIGNAL(saveScript(ScriptEditor::Script&)), SLOT(saveScriptEditing(ScriptEditor::Script&)));
        connect(script_enditor, SIGNAL(cancel()), SLOT(cancelScriptEditing()));
        connect(script_enditor, SIGNAL(presenterWidgetClosed()), SLOT(scriptEditorClosed()));
        launchPresenterWidget(script_enditor);
    }
    ScriptEditor::Script script(ui->textEditLoadParameter->toPlainText());
    script_enditor->setScript(script);
}

void ControUnitInstanceEditor::saveScriptEditing(ScriptEditor::Script& script_info) {
    ui->textEditLoadParameter->setText(script_info.getJSONDescription());
    script_enditor->closeTab();
    script_enditor = NULL;
}

void ControUnitInstanceEditor::cancelScriptEditing() {
    if(script_enditor) {
        script_enditor->closeTab();
        script_enditor = NULL;
    }
}

void ControUnitInstanceEditor::scriptEditorClosed() {
    script_enditor = NULL;
}
