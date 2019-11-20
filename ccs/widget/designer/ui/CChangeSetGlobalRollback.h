#ifndef CCHANGESETGLOBALROLLBACK_H
#define CCHANGESETGLOBALROLLBACK_H

#include "../ChaosBaseCommandButtonUI.h"

#include <QPushButton>

class CChangeSetGlobalRollback:
        public ChaosBaseCommandButtonUI {
    Q_OBJECT
public:
    CChangeSetGlobalRollback(QWidget *parent = nullptr);
private slots:
    void pressed();
private:
    QPushButton *push_button;
};
#endif // CCHANGESETGLOBALROLLBACK_H
