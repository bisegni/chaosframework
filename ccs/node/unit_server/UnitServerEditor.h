#ifndef UNITSERVEREDITOR_H
#define UNITSERVEREDITOR_H

#include "../../presenter/PresenterWidget.h"

#include <QItemSelection>
#include <QStringListModel>
#include <QStandardItemModel>

namespace Ui {
class UnitServerEditor;
}

class UnitServerEditor :
        public PresenterWidget
{
    Q_OBJECT
    const QString node_unique_id;
    //list model
    QStringListModel *list_model_cu_type;
    //query proxy
    chaos::metadata_service_client::api_proxy::unit_server::LoadUnloadControlUnit *us_load_unload_cu_proxy;
    chaos::metadata_service_client::api_proxy::unit_server::GetDescription *us_get_description_proxy;
    chaos::metadata_service_client::api_proxy::control_unit::SearchInstancesByUS *cu_si_proxy;
    chaos::metadata_service_client::api_proxy::control_unit::DeleteInstance *cu_di_proxy;

    //the lis tof the isntances setuped for the unit server
    std::vector<QSharedPointer<chaos::common::data::CDataWrapper> > instance_list;

    //add an instance to the table
    void fillTableWithInstance(QSharedPointer<chaos::common::data::CDataWrapper> cu_instance);
public:
    explicit UnitServerEditor(const QString& _node_unique_id);
    ~UnitServerEditor();

private slots:
    void customMenuRequested(QPoint pos);

    void updateAll();

    void on_pushButtonCreateNewInstance_clicked();

    void handleSelectionChanged(const QItemSelection& selection);

    void on_pushButtonUpdateAllInfo_clicked();

    void on_pushButtonRemoveInstance_clicked();

    void on_pushButtonEditInstance_clicked();

    void tableCurrentChanged(const QModelIndex &current,
                             const QModelIndex &previous);
    void tableSelectionChanged(const QItemSelection& selected,
                               const QItemSelection& unselected);
//----------control unti slot----------------
    void cuInstanceLoadSelected();
    void cuInstanceUnloadSelected();
    void cuInstanceInitSelected();
    void cuInstanceDeinitSelected();
    void cuInstanceStartSelected();
    void cuInstanceStopSelected();
    void on_pushButtonUpdateControlUnitType_clicked();

protected:
    void initUI();
    bool canClose();
    void onApiDone(const QString& tag,
                   QSharedPointer<chaos::common::data::CDataWrapper> api_result);

private:
    QStandardItemModel *table_model;
    Ui::UnitServerEditor *ui;
};

#endif // UNITSERVEREDITOR_H
