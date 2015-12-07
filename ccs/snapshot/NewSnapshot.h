#ifndef NEWSNAPSHOT_H
#define NEWSNAPSHOT_H

#include "NodeListModel.h"
#include "../presenter/PresenterWidget.h"
#include "../search/SearchNodeResult.h"

namespace Ui {
class NewSnapshot;
}

class NewSnapshot :
        public PresenterWidget {
    Q_OBJECT

protected:
    void initUI();
    bool isClosing();
    void onApiDone(const QString& tag,
                   QSharedPointer<chaos::common::data::CDataWrapper> api_result);
public:
    explicit NewSnapshot(QWidget *parent = 0);
    ~NewSnapshot();

private slots:
    void on_pushButtonAddNode_clicked();
    void on_pushButtonRemoveNode_clicked();
    void selectedNodes(const QString& tag,
                       const QVector<QPair<QString,QString> >& selected_nodes);
    void selectionChanged(const QItemSelection& start, const QItemSelection& end);
    void destroyed(QObject *destroyed_search_node);
    void on_pushButtonCreateSnapshot_clicked();

private:
    NodeListModel   node_model;
    SearchNodeResult *search_node;
    Ui::NewSnapshot *ui;
};

#endif // NEWSNAPSHOT_H
