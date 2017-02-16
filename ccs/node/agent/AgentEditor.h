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
        public PresenterWidget,
        protected chaos::metadata_service_client::event::alert::AlertAgentCheckProcessHandler {
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
    void contextualMenuActionTrigger(const QString& cm_title,
                                     const QVariant& cm_data);
    void handleAgentEvent(const std::string& _agent_uid,
                          const int32_t& _check_result);
private slots:
    void on_pushButtonUpdateList_clicked();

    void selectionChanged(const QItemSelection &selected, const QItemSelection &deselected);

    void doubleClicked(const QModelIndex& clickedIndex);

    void on_pushButtonEditAssociatedNode_clicked();

    void on_pushButtonRemoveAssociatedNode_clicked();

    void updateUI();
    void on_pushButtonStartCheckProcess_clicked();

private:
    QString agent_uid;
    Ui::AgentEditor *ui;
    chaos::service_common::data::agent::AgentInstance agent_insance;
    AgentNodeAssociatedListModel nodeAssociatedListModel;
};

#endif // AGENTEDITOR_H
