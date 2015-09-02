#ifndef PREFERENCEDIALOG_H
#define PREFERENCEDIALOG_H

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
private:
    QSettings settings;
    PreferenceMDSHostListModel list_model_mds_address;
    Ui::PreferenceDialog *ui;
};

#endif // PREFERENCEDIALOG_H
