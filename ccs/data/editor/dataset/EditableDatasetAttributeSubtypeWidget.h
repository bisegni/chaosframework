#ifndef EDITABLEDATASETATTRIBUTESUBTYPEWIDGET_H
#define EDITABLEDATASETATTRIBUTESUBTYPEWIDGET_H

#include "DatasAttributeSubtypeTableModel.h"

#include <chaos_service_common/data/dataset/DatasetAttribute.h>

#include <QWidget>
#include <QItemSelection>

namespace Ui {
class EditableDatasetAttributeSubtypeWidget;
}

class EditableDatasetAttributeSubtypeWidget :
        public QWidget
{
    Q_OBJECT

public:
    explicit EditableDatasetAttributeSubtypeWidget(QWidget *parent = 0);
    ~EditableDatasetAttributeSubtypeWidget();
    void setDatasetAttrbiute(chaos::service_common::data::dataset::DatasetAttribute *editable_dataset_attribute);
private slots:
    void on_pushButtonAdd_clicked();

    void on_pushButtonRemove_clicked();

    void tableSelectionChanged(const QItemSelection & selected, const QItemSelection & deselected);
private:

    DatasAttributeSubtypeTableModel dataset_subtypes_table_model;
    chaos::service_common::data::dataset::DatasetAttribute *editable_dataset_attribute_ptr;
    Ui::EditableDatasetAttributeSubtypeWidget *ui;
};

#endif // EDITABLEDATASETATTRIBUTESUBTYPEWIDGET_H
