#ifndef EDITABLEDATASETTABLEMODELEDITDIALOG_H
#define EDITABLEDATASETTABLEMODELEDITDIALOG_H

#include <QDialog>

#include <chaos/common/data/structured/DatasetAttribute.h>

namespace Ui {
class EditableDatasetTableModelEditDialog;
}

class EditableDatasetTableModelEditDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EditableDatasetTableModelEditDialog(QWidget *parent = 0);
    ~EditableDatasetTableModelEditDialog();
    void setDatsetAttribute(const chaos::common::data::structured::DatasetAttribute &dataset_attribute);
    const chaos::common::data::structured::DatasetAttribute& getDatasetAttribute();
private slots:
    void on_pushButtonOk_clicked();

    void on_pushButtonCancel_clicked();

    void on_comboBoxTypes_currentTextChanged(const QString &selected_type);

private:
    void fillUIFromAttribute();
    void fillAttributeFromUI();
    chaos::common::data::structured::DatasetAttribute editable_dataset_attribute;
    Ui::EditableDatasetTableModelEditDialog *ui;
};

#endif // EDITABLEDATASETTABLEMODELEDITDIALOG_H
