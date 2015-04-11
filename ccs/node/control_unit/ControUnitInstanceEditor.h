#ifndef CONTROUNITINSTANCEEDITOR_H
#define CONTROUNITINSTANCEEDITOR_H

#include "../../presenter/PresenterWidget.h"

#include <QWidget>
#include <QItemSelection>
#include <QModelIndexList>
#include <QStandardItemModel>

namespace Ui {
class ControUnitInstanceEditor;
}

class ControUnitInstanceEditor :
        public PresenterWidget
{
    Q_OBJECT
   // QString unit_server_uid;
   // QString control_unit_type;
   // QString control_unit_uid;

    bool is_in_editing;

    //table model
    QStandardItemModel *table_model_driver_spec;
    QStandardItemModel *table_model_dataset_attribute_setup;
public:
    //!Create instance with unit server and cu type
    explicit ControUnitInstanceEditor(const QString& unit_server_uid,
                                      const QString& control_unit_type);
    //!edit the control unit
    explicit ControUnitInstanceEditor(const QString& unit_server_uid,
                                      const QString &control_unit_uid,
                                      bool edit_instance);
    ~ControUnitInstanceEditor();

    //read the ui and preapre the api proxy with the value
    chaos::metadata_service_client::api_proxy::control_unit::SetInstanceDescription *prepareSetInstanceApi();

    void fillUIFromInstanceInfo(QSharedPointer<chaos::common::data::CDataWrapper> api_result);
protected:
    void initUI();
    bool canClose();
    void onApiDone(QString tag,
                   QSharedPointer<chaos::common::data::CDataWrapper> api_result);

private slots:
    void on_pushButtonSaveInstance_clicked();

    void on_pushButtonAddDriverDescription_clicked();

    void on_pushButtonEditDriverDescription_clicked();

    void on_pushButtonDeleteDriverDescription_clicked();

    void tableDriverDescriptionCurrentChanged(const QModelIndex &current,
                                              const QModelIndex &previous);
    void tableDriverDescriptionSelectionChanged(const QItemSelection& selected,
                                                const QItemSelection& unselected);
    void on_pushButtonAddAttribute_clicked();

    void on_pushButtonEditAttribute_clicked();

    void on_pushButtonRemoveAttribute_clicked();

    void addNewDriverDescription(const QString& driver_name,
                                 const QString& driver_version,
                                 const QString& driver_init_parameter);
    void updateDriverDescription(int current_row,
                                 const QString& driver_name,
                                 const QString& driver_version,
                                 const QString& driver_init_parameter);

    void addAttributeDescription(const QString& attribute_name,
                                 const QString& attribute_default_value,
                                 const QString& attribute_default_max_value,
                                 const QString& attribute_default_min_value);
    void updateAttributeDescription(int current_row,
                                    const QString& attribute_name,
                                    const QString& attribute_default_value,
                                    const QString& attribute_default_max_value,
                                    const QString& attribute_default_min_value);
private:
    //set instance api proxy
    chaos::metadata_service_client::api_proxy::control_unit::SetInstanceDescription *set_inst_desc_proxy;
    chaos::metadata_service_client::api_proxy::control_unit::GetInstance *get_instance_api_proxy;
    Ui::ControUnitInstanceEditor *ui;
};

#endif // CONTROUNITINSTANCEEDITOR_H
