#include "EditableSubtypeListWidget.h"
#include "ui_EditableSubtypeListWidget.h"
#include "../../delegate/ComboBoxDelegate.h"

using namespace chaos::common::data::structured;

EditableSubtypeListWidget::EditableSubtypeListWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::EditableSubtypeListWidget){
    ui->setupUi(this);

    //add model and delegate to table for subtypes
    QStringList subtypes_for_combo;
    subtypes_for_combo << tr("Boolean")<< tr("Char")<< tr("Int8")<< tr("Int16")<< tr("Int32")<< tr("Int64")<< tr("Double")<< tr("String");
    ui->tableViewSubtypes->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableViewSubtypes->setModel(&subtypes_table_model);
    ui->tableViewSubtypes->setItemDelegateForColumn(0, new ComboBoxDelegate(subtypes_for_combo,
                                                                            this));
    connect(ui->tableViewSubtypes->selectionModel(),
            SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
            SLOT(tableSelectionChanged(QItemSelection,QItemSelection)));
}
void EditableSubtypeListWidget::setSubtypeListPtr(DatasetSubtypeList *_editable_subtypes_list) {
    if(_editable_subtypes_list == NULL) return;
    subtypes_table_model.setSubtypesList(_editable_subtypes_list);
}

EditableSubtypeListWidget::~EditableSubtypeListWidget() {
    delete ui;
}

void EditableSubtypeListWidget::on_pushButtonAdd_clicked() {
    subtypes_table_model.addNew();
}

void EditableSubtypeListWidget::on_pushButtonRemove_clicked() {
    foreach (QModelIndex element, ui->tableViewSubtypes->selectionModel()->selectedRows()) {
        subtypes_table_model.removeAtIndex(element.row());
    }
}

void EditableSubtypeListWidget::tableSelectionChanged(const QItemSelection & selected, const QItemSelection & deselected) {
    ui->pushButtonRemove->setEnabled(selected.size() > 0);
}
