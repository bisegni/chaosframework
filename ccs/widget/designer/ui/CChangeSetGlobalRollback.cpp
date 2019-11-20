#include "CChangeSetGlobalRollback.h"

#include <QBoxLayout>

CChangeSetGlobalRollback::CChangeSetGlobalRollback(QWidget *parent):
    ChaosBaseCommandButtonUI(parent) {
    push_button = new QPushButton(this);
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setSpacing(-1);
    layout->setContentsMargins(0,0,0,0);
    push_button->setText("Rollback");
    layout->addWidget(push_button);
    setLayout(layout);

    connect(push_button,
            &QPushButton::pressed,
            this,
            &CChangeSetGlobalRollback::pressed);
}

void CChangeSetGlobalRollback::pressed() {
    emit rollbackChangeSet();
}
