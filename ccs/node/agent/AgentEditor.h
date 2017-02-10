#ifndef AGENTEDITOR_H
#define AGENTEDITOR_H

#include "../../presenter/PresenterWidget.h"
#include "../../data/AgentNodeAssociatedListModel.h"

#include <QWidget>
#include <QItemSelection>

namespace Ui {
class AgentEditor;
}

class AgentEditor :
        public PresenterWidget {
    Q_OBJECT

public:
    explicit AgentEditor(const QString& _agent_uid,
                         QWidget *parent = 0);
    ~AgentEditor();
protected:
    void initUI();
    bool isClosing();
    void onApiDone(const QString& tag,
                   QSharedPointer<chaos::common::data::CDataWrapper> api_result);
private slots:
    void on_pushButtonUpdateList_clicked();

    void selectionChanged(const QItemSelection &selected, const QItemSelection &deselected);

    void doubleClicked(const QModelIndex& clickedIndex);

    void on_pushButtonEditAssociatedNode_clicked();

    void on_pushButtonRemoveAssociatedNode_clicked();

private:
    QString agent_uid;
    Ui::AgentEditor *ui;
    AgentNodeAssociatedListModel nodeAssociatedListModel;
};

#endif // AGENTEDITOR_H
