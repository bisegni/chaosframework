#ifndef PREFERENCEDIALOG_H
#define PREFERENCEDIALOG_H

#include "preference_types.h"
#include "PreferenceMDSHostListModel.h"

#include <QDialog>
#include <QSettings>
#include <QAbstractButton>


namespace Ui {
class PreferenceDialog;
}

class PreferenceDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PreferenceDialog(QWidget *parent = 0);
    ~PreferenceDialog();
signals:
    void changedConfiguration();
private slots:
    void on_pushButtonAddNewMDSRpcEndpoint_clicked();

    void on_pushButtonRemoveSelectedMDSRpcEndpoint_clicked();

    void loadAllPreference();
    void saveAllPreference();
    void clicked(QAbstractButton *clicked_button);
    void loadMDSConfiguration(const QString& configuration_name);
    void updateMDSConfiguration();
    void on_pushButtonAddNewConfiguration_clicked();
    void on_pushButtonRemoveConfiguration_clicked();
    void on_comboBoxConfigurations_currentTextChanged(const QString &selected_configuration);

    void on_pushButtonSave_accepted();

    void on_pushButtonNetworkSave_clicked();

private:
    QSettings settings;
    PreferenceMDSHostListModel list_model_mds_address;
    Ui::PreferenceDialog *ui;

    void updatePreverence();
};

#endif // PREFERENCEDIALOG_H
