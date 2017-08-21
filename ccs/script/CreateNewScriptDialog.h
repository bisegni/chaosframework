#ifndef CREATENEWSCRIPTDIALOG_H
#define CREATENEWSCRIPTDIALOG_H

#include <QDialog>
#include <chaos_metadata_service_client/ChaosMetadataServiceClient.h>

namespace Ui {
class CreateNewScriptDialog;
}

class CreateNewScriptDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CreateNewScriptDialog(QWidget *parent = 0);
    ~CreateNewScriptDialog();
    void fillScript(chaos::service_common::data::script::Script &script_description);
private slots:


    void on_pushButtonCreateScript_clicked();

    void on_pushButtonCancel_clicked();

private:
    Ui::CreateNewScriptDialog *ui;
};

#endif // CREATENEWSCRIPTDIALOG_H
