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
    int current_page;
    int current_page_length;

    //query proxy
    chaos::metadata_service_client::api_proxy::node::NodeSearch *ns_proxy;
    //api has ben called successfully
    void onApiDone(QString tag,
                        QSharedPointer<chaos::common::data::CDataWrapper> api_result);
protected:
    void initUI();
    bool canClose();

public:
    explicit SearchNodeResult();
    ~SearchNodeResult();

signals:

private slots:
    void on_pushButtonNextPage_clicked();

    void on_pushButtonPrevPage_clicked();

    void on_pushButtonOpenNodeInEditor_clicked();

    void on_tableViewResult_clicked(const QModelIndex &index);

    void on_pushButtonStartSearch_clicked();

private:
    Ui::SearchNodeResult *ui;
    QStandardItemModel *table_model;
};

#endif // SEARCHNODERESULT_H
