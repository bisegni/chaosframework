#include "EditableDatasetAttributeSubtypeWidget.h"
#include "ui_EditableDatasetAttributeSubtypeWidget.h"
#include "../../delegate/ComboBoxDelegate.h"

EditableDatasetAttributeSubtypeWidget::EditableDatasetAttributeSubtypeWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::EditableDatasetAttributeSubtypeWidget),
    editable_dataset_attribute_ptr(NULL){
    ui->setupUi(this);

    //add model and delegate to table for subtypes
    QStringList subtypes_for_combo;
    subtypes_for_combo << tr("Boolean")<< tr("Char")<< tr("Int8")<< tr("Int16")<< tr("Int32")<< tr("Int64")<< tr("Double")<< tr("String");
    ui->tableViewSubtypes->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->tableViewSubtypes->setModel(&dataset_subtypes_table_model);
    ui->tableViewSubtypes->setItemDelegateForColumn(0, new ComboBoxDelegate(subtypes_for_combo,
                                                                            this));
    connect(ui->tableViewSubtypes->selectionModel(),
            SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
            SLOT(tableSelectionChanged(QItemSelection,QItemSelection)));
}
void EditableDatasetAttributeSubtypeWidget::setDatasetAttrbiute(chaos::service_common::data::dataset::DatasetAttribute *editable_dataset_attribute) {
    CHAOS_ASSERT(editable_dataset_attribute)
    if(editable_dataset_attribute == NULL) return;
    editable_dataset_attribute_ptr = editable_dataset_attribute;
    dataset_subtypes_table_model.setDatasetSubtypesList(&editable_dataset_attribute_ptr->binary_subtype_list);
}

EditableDatasetAttributeSubtypeWidget::~EditableDatasetAttributeSubtypeWidget() {
    delete ui;
}

void EditableDatasetAttributeSubtypeWidget::on_pushButtonAdd_clicked() {
    dataset_subtypes_table_model.addNew();
}

void EditableDatasetAttributeSubtypeWidget::on_pushButtonRemove_clicked() {
    foreach (QModelIndex element, ui->tableViewSubtypes->selectionModel()->selectedRows()) {
            dataset_subtypes_table_model.removeAtIndex(element.row());
    }
}

void EditableDatasetAttributeSubtypeWidget::tableSelectionChanged(const QItemSelection & selected, const QItemSelection & deselected) {
    ui->pushButtonRemove->setEnabled(selected.size() > 0);
}
