#ifndef AGENTNODEASSOCIATIONEDITOR_H
#define AGENTNODEASSOCIATIONEDITOR_H

#include "../../presenter/PresenterWidget.h"

#include <QWidget>

namespace Ui {
class AgentNodeAssociationEditor;
}

class AgentNodeAssociationEditor :
        public PresenterWidget {
    Q_OBJECT

public:
    explicit AgentNodeAssociationEditor(const QString& _agent_uid,
                                        const QString& _associated_node_uid,
                                        QWidget *parent = 0);
    ~AgentNodeAssociationEditor();
protected:
    void initUI();
    bool isClosing();
    void onApiDone(const QString& tag,
                   QSharedPointer<chaos::common::data::CDataWrapper> api_result);
private slots:
    void on_pushButtonSaveAssociation_clicked();
    void updateUI();
    void on_pushButtonUpdateAssociationInformation_clicked();

private:
    QString agent_uid;
    Ui::AgentNodeAssociationEditor *ui;
    chaos::service_common::data::agent::AgentAssociation association;
};

#endif // AGENTNODEASSOCIATIONEDITOR_H
