#ifndef CCHANGESETGLOBALCOMMIT_H
#define CCHANGESETGLOBALCOMMIT_H

#include "../ChaosBaseCommandButtonUI.h"

#include <QPushButton>

class CChangeSetGlobalCommit:
        public ChaosBaseCommandButtonUI {
    Q_OBJECT
public:
    CChangeSetGlobalCommit(QWidget *parent = nullptr);
private slots:
    void pressed();
private:
    QPushButton *push_button;
};

#endif // CCHANGESETGLOBALCOMMIT_H
