#include "ScriptSignalDialog.h"
#include "ui_ScriptSignalDialog.h"

ScriptSignalDialog::ScriptSignalDialog(const QStringList &signal_list, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ScriptSignalDialog) {
    ui->setupUi(this);
    ui->comboBoxSignals->addItems(signal_list);
}

ScriptSignalDialog::~ScriptSignalDialog()
{
    delete ui;
}
