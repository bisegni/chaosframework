#include "WidgetUtility.h"

WidgetUtility::WidgetUtility(WidgetUtilityhandler *_handler, QObject *parent):
    QObject(parent),
    handler(_handler){}

WidgetUtility::~WidgetUtility(){}

void WidgetUtility::cmRegisterActions(QWidget *contextual_menu_parent,
                                      const QMap<QString, QVariant>& cm_map) {
    if(!contextual_menu_parent) return;
    contextual_menu_parent->setContextMenuPolicy(Qt::ActionsContextMenu);


    QMapIterator<QString, QVariant> it(cm_map);
    while(it.hasNext()) {
        it.next();
        const QString action_title = it.key();
        QAction *action = new QAction(action_title, contextual_menu_parent);
        action->setData(it.value());
        contextual_menu_parent->addAction(action);
        connect(action,
                SIGNAL(triggered()),
                this,
                SLOT(cmTriggerHandler()));
    }
}

QAction *WidgetUtility::cmGetAction(QWidget *parent,
                                    const QString& action_name) {
    foreach (QAction *act,  parent->actions()) {
        if(act->text().compare(action_name) == 0) {
            return act;
        }
    }
    return NULL;
}

void WidgetUtility::cmActionSetEnable(QWidget *parent,
                                      const QString& action_name,
                                      bool enabled) {
    QAction *act = NULL;
    if((act = cmGetAction(parent, action_name))){
        act->setEnabled(enabled);
    }
}

void WidgetUtility::cmActionSetVisible(QWidget *parent,
                                       const QString& action_name,
                                       bool visible) {
    QAction *act = NULL;
    if((act = cmGetAction(parent, action_name))){
        act->setVisible(visible);
    }
}

void WidgetUtility::cmActionSetData(QWidget *parent,
                                    const QString& action_name,
                                    const QVariant& action_data) {
    QAction *act = NULL;
    if((act = cmGetAction(parent, action_name))){
        act->setData(action_data);
    }
}

void WidgetUtility::cmTriggerHandler() {
    Q_ASSERT(handler);
    QAction* cm_action = qobject_cast<QAction*>(sender());
    handler->cmActionTrigger(cm_action->text(), cm_action->data());
}
