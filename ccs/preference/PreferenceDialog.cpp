#include "PreferenceDialog.h"
#include "ui_PreferenceDialog.h"

#include <QInputDialog>


PreferenceDialog::PreferenceDialog(QWidget *parent) :
    QDialog(parent),
    settings("it.infn", "ccs"),
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
    updatePreverence();
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

void PreferenceDialog::updatePreverence() {
    if (settings.childGroups().contains("network", Qt::CaseInsensitive)){
        //we have old network preference
        //convertion
        settings.beginGroup("network");
        settings.remove("");
        settings.endGroup();
        settings.sync();
    }
}

void PreferenceDialog::loadAllPreference() {
    QStringList cy_type_list;
    settings.beginGroup(PREFERENCE_NETWORK_GROUP_NAME);
    QStringList configurations = settings.childGroups();
    //write current configurations
    ui->comboBoxConfigurations->clear();
    ui->comboBoxConfigurations->addItems(configurations);
    settings.endGroup();

    loadMDSConfiguration(ui->comboBoxConfigurations->currentText());
}

void PreferenceDialog::saveAllPreference() {
//    settings.beginGroup(PREFERENCE_NETWORK_GROUP_NAME);
//    settings.sync();
//    //emit signal for changed preference
//    emit changedConfiguration();
}

void PreferenceDialog::loadMDSConfiguration(const QString& configuration_name) {
    settings.beginGroup(QString("%1/%2").arg(QString(PREFERENCE_NETWORK_GROUP_NAME), configuration_name));
    //read current configurations
    QStringList cy_type_list;
    int size = settings.beginReadArray("mds_address");
    for (int i = 0; i < size; ++i) {
        settings.setArrayIndex(i);
        cy_type_list.append(settings.value("address").toString());
    }
    list_model_mds_address.setStringList(cy_type_list);
    settings.endArray();
    settings.endGroup();
}

void PreferenceDialog::updateMDSConfiguration() {
    if(ui->comboBoxConfigurations->currentIndex() < 0) return;
    const QString configuration_name = ui->comboBoxConfigurations->currentText();
    //write configuration
    settings.beginGroup(QString("%1/%2").arg(QString(PREFERENCE_NETWORK_GROUP_NAME), configuration_name));
    //scan all mds ip
    settings.beginWriteArray("mds_address");
    const QStringList& cy_type_list = list_model_mds_address.stringList();
    for (int i = 0; i < cy_type_list.size(); ++i) {
        settings.setArrayIndex(i);
        settings.setValue("address", cy_type_list.at(i));
    }
    settings.endArray();
    //close group
    settings.endGroup();
    settings.sync();
}

void PreferenceDialog::on_pushButtonAddNewMDSRpcEndpoint_clicked() {
    bool ok;
    const QString new_mds_address = QInputDialog::getText(this, tr("New metadata server address"),
                                                        tr("MDS address:"),
                                                        QLineEdit::Normal,
                                                          tr("127.0.0.1:5000"),
                                                        &ok);
    if (ok && !new_mds_address.isEmpty()) {
        list_model_mds_address.addMDSServer(new_mds_address);
    }
}

void PreferenceDialog::on_pushButtonRemoveSelectedMDSRpcEndpoint_clicked() {
    QModelIndexList selected_row =  ui->listViewMDSAddress->selectionModel()->selectedRows();
    foreach(QModelIndex selected, selected_row) {
        list_model_mds_address.removeRow(selected.row());
    }
}

void PreferenceDialog::on_pushButtonAddNewConfiguration_clicked() {
    bool ok;
    const QString configuration_name = QInputDialog::getText(this, tr("New configuration name"),
                                                        tr("MDS Configuration:"),
                                                        QLineEdit::Normal,
                                                        tr("New MDS Configuration"),
                                                        &ok);
    if (ok && !configuration_name.isEmpty()) {
        settings.beginGroup(QString("%1/%2").arg(QString(PREFERENCE_NETWORK_GROUP_NAME), configuration_name));
        settings.endGroup();
        settings.sync();
        ui->comboBoxConfigurations->addItem(configuration_name);
    }
}

void PreferenceDialog::on_pushButtonRemoveConfiguration_clicked() {
    int current_index = ui->comboBoxConfigurations->currentIndex();
    if(current_index < 0) return;
    settings.beginGroup(QString("%1/%2").arg(QString(PREFERENCE_NETWORK_GROUP_NAME), ui->comboBoxConfigurations->currentText()));
    settings.remove(""); //removes the group, and all it keys
    settings.endGroup();
    ui->comboBoxConfigurations->removeItem(current_index);
}

void PreferenceDialog::on_comboBoxConfigurations_currentTextChanged(const QString &selected_configuration) {
    //then load selected
    loadMDSConfiguration(selected_configuration);
}

void PreferenceDialog::on_pushButtonSave_accepted(){

}

void PreferenceDialog::on_pushButtonNetworkSave_clicked() {
    updateMDSConfiguration();
}
