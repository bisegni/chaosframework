#ifndef SEARCHNODERESULT_H
#define SEARCHNODERESULT_H

#include "../presenter/PresenterWidget.h"
#include <QWidget>
#include <QTableView>
#include <QItemDelegate>
#include <QStandardItemModel>

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
    //api has ben called successfully
    void onApiDone(QString tag,
                        QSharedPointer<chaos::common::data::CDataWrapper> api_result);
protected:
    void getSearchTypeAsString(QString& type_description);
    void initUI();
    bool canClose();

public:
    explicit SearchNodeResult(int _search_type, const QString& _search_criteria);
    ~SearchNodeResult();

signals:

private slots:
    void on_pushButtonNextPage_clicked();

    void on_pushButtonPrevPage_clicked();

    void on_pushButtonOpenNodeInEditor_clicked();

    void on_tableViewResult_clicked(const QModelIndex &index);


private:
    Ui::SearchNodeResult *ui;
    QStandardItemModel *table_model;
};

#endif // SEARCHNODERESULT_H
