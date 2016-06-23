#ifndef SEARCHNODERESULT_H
#define SEARCHNODERESULT_H

#include "../presenter/PresenterWidget.h"

#include <QTimer>
#include <QWidget>
#include <QTableView>
#include <QItemDelegate>
#include <QStandardItemModel>
#include <QList>

namespace Ui {
class SearchNodeResult;
}

typedef enum SearchNodeType {
    SNT_ALL_TYPE,
    SNT_UNIT_SERVER,
    SNT_CONTROL_UNIT,
    SNT_SCRIPTABLE_EXECUTION_UNIT
} SearchNodeType;

class SearchNodeResult :
        public PresenterWidget
{
    Q_OBJECT
    //! search property

    QString tag;
    QList<SearchNodeType> selectable_types;
    int current_page_length;
    bool selection_mode;

    //api has ben called successfully
    void onApiDone(const QString& tag,
                   QSharedPointer<chaos::common::data::CDataWrapper> api_result);

    //-------------default node cm event
    void startHealtMonitorAction();
    void stopHealtMonitorAction();
protected:
    void initUI();
    bool isClosing();
    void contextualMenuActionTrigger(const QString& cm_title,
                                     const QVariant& cm_data);
public:
    explicit SearchNodeResult(bool _selection_mode = false,
                              const QString& _tag = QString());
    SearchNodeResult(bool _selection_mode,
                     const QList<SearchNodeType>& _selectable_types,
                     const QString& _tag = QString());
    ~SearchNodeResult();

signals:
    void selectedNodes(const QString& tag,
                       const QVector<QPair<QString,QString> >& selected_nodes);
private slots:
    void on_tableViewResult_clicked(const QModelIndex &index);

    void on_pushButtonStartSearch_clicked();

    void on_pushButtonActionOnSelected_clicked();

    void on_tableViewResult_doubleClicked(const QModelIndex &index);

    void on_lineEditSearchCriteria_textEdited(const QString &search_string);

private:
    QTimer search_timer;
    Ui::SearchNodeResult *ui;
    QStandardItemModel *table_model;
};

#endif // SEARCHNODERESULT_H
