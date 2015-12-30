#include "PreferenceDialog.h"
#include "ui_PreferenceDialog.h"

PreferenceDialog::PreferenceDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PreferenceDialog) {
    ui->setupUi(this);

    ui->listViewMDSAddress->setModel(&list_model_mds_address);

    connect(ui->buttonBox,
            SIGNAL(accepted()),
            SLOT(saveAllPreference()));
    connect(ui->buttonBox,
            SIGNAL(rejected()),
            SLOT(close()));
    connect(ui->buttonBox,
            SIGNAL(clicked(QAbstractButton*)),
            SLOT(clicked(QAbstractButton*)));
    loadAllPreference();
}

PreferenceDialog::~PreferenceDialog() {
    delete ui;
}

void PreferenceDialog::clicked(QAbstractButton *clicked_button) {
    if(ui->buttonBox->buttonRole(clicked_button) == QDialogButtonBox::ResetRole) {
        //reset all the user changes
        loadAllPreference();
    }
}

void PreferenceDialog::loadAllPreference() {
    QStringList cy_type_list;

    settings.beginGroup("network");
    int size = settings.beginReadArray("mds_address");
    for (int i = 0; i < size; ++i) {
        settings.setArrayIndex(i);
        cy_type_list.append(settings.value("address").toString());
    }
    list_model_mds_address.setStringList(cy_type_list);
    settings.endArray();
    settings.endGroup();
}

void PreferenceDialog::saveAllPreference() {
    settings.beginGroup("network");
    settings.beginWriteArray("mds_address");
    const QStringList& cy_type_list = list_model_mds_address.stringList();
    for (int i = 0; i < cy_type_list.size(); ++i) {
        settings.setArrayIndex(i);
        settings.setValue("address", cy_type_list.at(i));
    }
    settings.endArray();
    settings.endGroup();
    settings.sync();
    //emit signal for changed preference
    emit changedConfiguration();
}

void PreferenceDialog::on_pushButtonAddNewMDSRpcEndpoint_clicked() {
    list_model_mds_address.insertRow(list_model_mds_address.rowCount());
    QModelIndex index = list_model_mds_address.index(list_model_mds_address.rowCount()-1);
    list_model_mds_address.setData(index, tr("127.0.0.1:5000"));
}

void PreferenceDialog::on_pushButtonRemoveSelectedMDSRpcEndpoint_clicked() {
    QModelIndexList selected_row =  ui->listViewMDSAddress->selectionModel()->selectedRows();
    foreach(QModelIndex selected, selected_row) {
        list_model_mds_address.removeRow(selected.row());
    }
}
