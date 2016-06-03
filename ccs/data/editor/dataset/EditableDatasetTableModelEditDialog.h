#ifndef EDITABLEDATASETTABLEMODELEDITDIALOG_H
#define EDITABLEDATASETTABLEMODELEDITDIALOG_H

#include <QDialog>

#include <chaos_service_common/data/dataset/DatasetAttribute.h>

namespace Ui {
class EditableDatasetTableModelEditDialog;
}

class EditableDatasetTableModelEditDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EditableDatasetTableModelEditDialog(QWidget *parent = 0);
    ~EditableDatasetTableModelEditDialog();
    void setDatsetAttribute(const chaos::service_common::data::dataset::DatasetAttribute &dataset_attribute);
    const chaos::service_common::data::dataset::DatasetAttribute& getDatasetAttribute();
private slots:
    void on_pushButtonOk_clicked();

    void on_pushButtonCancel_clicked();

private:
    void fillUIFromAttribute();
    void fillAttributeFromUI();
    chaos::service_common::data::dataset::DatasetAttribute editable_dataset_attribute;
    Ui::EditableDatasetTableModelEditDialog *ui;
};

#endif // EDITABLEDATASETTABLEMODELEDITDIALOG_H
