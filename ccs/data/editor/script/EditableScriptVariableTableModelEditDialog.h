#ifndef EDITABLESCRIPTVARIABLETABLEMODELEDITDIALOG_H
#define EDITABLESCRIPTVARIABLETABLEMODELEDITDIALOG_H

#include <QDialog>

namespace Ui {
class EditableScriptVariableTableModelEditDialog;
}

class EditableScriptVariableTableModelEditDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EditableScriptVariableTableModelEditDialog(QWidget *parent = 0);
    explicit EditableScriptVariableTableModelEditDialog(const chaos::service_common::data::dataset::AlgorithmVariable &variable,QWidget *parent = 0);
    ~EditableScriptVariableTableModelEditDialog();
    void setVariable(const chaos::service_common::data::dataset::AlgorithmVariable &variable);
    const chaos::service_common::data::dataset::AlgorithmVariable& getVariable();

private slots:
    void on_pushButtonOk_clicked();
    void on_pushButtonCancel_clicked();
    void on_comboBoxTypes_currentTextChanged(const QString &selected_type);

private:
    void fillUIFromVariable();
    void fillVariableFromUI();
    chaos::service_common::data::dataset::AlgorithmVariable editable_variable;
    Ui::EditableScriptVariableTableModelEditDialog *ui;
};

#endif // EDITABLESCRIPTVARIABLETABLEMODELEDITDIALOG_H
