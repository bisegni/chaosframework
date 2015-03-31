#ifndef SEARCHNODERESULT_H
#define SEARCHNODERESULT_H

#include "../presenter/PresenterWidget.h"

#include <QWidget>
#include <QTableView>
#include <QItemDelegate>
#include <QStandardItemModel>

#include <ChaosMetadataServiceClient/ChaosMetadataServiceClient.h>

namespace Ui {
class SearchNodeResult;
}

class SearchNodeResult :
public PresenterWidget
{
    Q_OBJECT

    //! search property

    int search_type;
    QString search_criteria;

    int current_page;
    int current_page_length;

    //query proxy
    chaos::metadata_service_client::api_proxy::node::NodeSearch *ns_proxy;


protected:
    void getSearchTypeAsString(QString& type_description);
    void updateUI();
    bool canClose();

public:
    explicit SearchNodeResult(int _search_type, const QString& _search_criteria);
    ~SearchNodeResult();

signals:
    void startUpdateResult();

private slots:

    void updateResult();

    void on_pushButtonNextPage_clicked();

    void on_pushButtonPrevPage_clicked();

    void on_pushButtonOpenNodeInEditor_clicked();

private:
    Ui::SearchNodeResult *ui;
    QStandardItemModel *table_model;
};

#endif // SEARCHNODERESULT_H
