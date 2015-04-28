#ifndef DATASERVICEEDITOR_H
#define DATASERVICEEDITOR_H

#include "../../presenter/PresenterWidget.h"

#include <QItemSelection>
#include <QStandardItemModel>
namespace Ui {
class DataServiceEditor;
}

class DataServiceEditor :
        public PresenterWidget
{
    Q_OBJECT

public:
    explicit DataServiceEditor();
    ~DataServiceEditor();
protected:
    void initUI();
    bool canClose();
    void onApiDone(const QString& tag,
                   QSharedPointer<chaos::common::data::CDataWrapper> api_result);
    void fillTableDS(QSharedPointer<chaos::common::data::CDataWrapper> api_result);
    void fillDataServiceAssocaition(const QString& data_service,
                                    QSharedPointer<chaos::common::data::CDataWrapper> api_result);
private slots:
    void handleNewDataService(const QString& unique_id,
                              const QString& direct_io_address,
                              int endpoint);
    void handleUpdateDataService(int current_row,
                                 const QString& unique_id,
                                 const QString& direct_io_address,
                                 int endpoint);

    void on_pushButtonAddDataService_clicked();
    //-------------list and table event-------------------------
    void tableDSCustomMenuRequested(QPoint pos);

    void loadCUAssociationForDataService();

    void tableDSCurrentChanged(const QModelIndex &current,
                               const QModelIndex &previous);
    void tableDSSelectionChanged(const QItemSelection& selected,
                                 const QItemSelection& unselected);
    void tableAssocCurrentChanged(const QModelIndex &current,
                                  const QModelIndex &previous);
    void tableAssocSelectionChanged(const QItemSelection& selected,
                                    const QItemSelection& unselected);
    void on_pushButtoneditDataService_clicked();

    void on_pushButtonUpdateDSTable_clicked();

    void on_pushButtonRemoveDataService_clicked();

    void on_pushButtonAssociateNewControlUnit_clicked();

    //slot for handle the selction of the node to attacch to the dataservice exposed on tag field
    void handleSelectedNodes(const QString& tag,
                             const QVector<QPair<QString,QString> >& selected_nodes);

    void on_pushButtonRemoveAssociatedControlUnit_clicked();

    void on_pushButtonUpdateAssociationList_clicked();

private:
    Ui::DataServiceEditor *ui;
    QStandardItemModel *table_model_ds;
    QStandardItemModel *table_model_acu;
};

#endif // DATASERVICEEDITOR_H
