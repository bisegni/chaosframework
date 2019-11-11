#ifndef CHAOSBASECOMMANDBUTTONUI_H
#define CHAOSBASECOMMANDBUTTONUI_H

#include <QFrame>
#include "ChaosBaseUI.h"
class ChaosBaseCommandButtonUI:
        public ChaosBaseUI {
    Q_OBJECT
public:
    ChaosBaseCommandButtonUI(QWidget *parent = nullptr);
signals:
    void commitChangeSet();
    void rollbackChangeSet();
};

#endif // CHAOSBASECOMMANDBUTTONUI_H
