#ifndef CHAOSBASECOMMANDBUTTONUI_H
#define CHAOSBASECOMMANDBUTTONUI_H

#include <QFrame>

class ChaosBaseCommandButtonUI:
        public QFrame {
    Q_OBJECT
public:
    ChaosBaseCommandButtonUI(QWidget *parent = nullptr);
signals:
    void commitChangeSet();
    void rollbackChangeSet();
};

#endif // CHAOSBASECOMMANDBUTTONUI_H
