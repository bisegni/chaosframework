#ifndef CHAOSBASEUI_H
#define CHAOSBASEUI_H

#include <QFrame>
#include <QJsonObject>

class ChaosBaseUI:
        public QFrame {
    Q_OBJECT
public:
    ChaosBaseUI(QWidget *parent = nullptr);
    virtual QJsonObject serialize();
    virtual void deserialize(QJsonObject& serialized_data);
};

#endif // CHAOSBASEUI_H
