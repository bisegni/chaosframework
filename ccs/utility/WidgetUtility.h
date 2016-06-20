#ifndef WIDGETUTILITY_H
#define WIDGETUTILITY_H

#include <QObject>
#include <QAction>
#include <QVariant>

class WidgetUtilityhandler {
public:
    virtual void cmActionTrigger(const QString& cm_title,
                                 const QVariant& cm_data) = 0;
};

class WidgetUtility:
        public QObject{
    Q_OBJECT

    WidgetUtilityhandler *handler;
protected slots:
    void cmTriggerHandler();
public:
    WidgetUtility(WidgetUtilityhandler *_handler,
                  QObject *parent = 0);
    ~WidgetUtility();
    void cmRegisterActions(QWidget *contextual_menu_parent,
                           const QMap<QString, QVariant> &cm_map);

    QAction *cmGetAction(QWidget *parent,
                         const QString& action_name);

    void cmActionSetEnable(QWidget *parent,
                           const QString& action_name,
                           bool enabled);

    void cmActionSetVisible(QWidget *parent,
                            const QString& action_name,
                            bool visible);

    void cmActionSetData(QWidget *parent,
                         const QString& action_name,
                         const QVariant& action_data);
};

#endif // WIDGETUTILITY_H
