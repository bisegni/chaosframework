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

    QString tag;
    int current_page;
    int current_page_length;
    bool selection_mode;

    //query proxy
    chaos::metadata_service_client::api_proxy::node::NodeSearch *ns_proxy;
    //api has ben called successfully
    void onApiDone(const QString& tag,
                   QSharedPointer<chaos::common::data::CDataWrapper> api_result);

    //-------------default node cm event
    void startHealtMonitorAction();
    void stopHealtMonitorAction();
protected:
    void initUI();
    bool canClose();

public:
    explicit SearchNodeResult(bool _selection_mode = false,
                              const QString& _tag = QString());
    ~SearchNodeResult();

signals:
    void selectedNodes(const QString& tag,
                       const QVector<QPair<QString,QString> >& selected_nodes);
private slots:
    void on_pushButtonNextPage_clicked();

    void on_pushButtonPrevPage_clicked();

    void on_tableViewResult_clicked(const QModelIndex &index);

    void on_pushButtonStartSearch_clicked();

    void on_pushButtonActionOnSelected_clicked();

private:
    Ui::SearchNodeResult *ui;
    QStandardItemModel *table_model;
};

#endif // SEARCHNODERESULT_H
