#include "CChangeSetGlobalCommit.h"

#include <QBoxLayout>

CChangeSetGlobalCommit::CChangeSetGlobalCommit(QWidget *parent):
    ChaosBaseCommandButtonUI(parent) {
    push_button = new QPushButton(this);
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setSpacing(-1);
    layout->setMargin(0);
    push_button->setText("Commit Changeset");
    layout->addWidget(push_button);
    setLayout(layout);

    connect(push_button,
            &QPushButton::pressed,
            this,
            &CChangeSetGlobalCommit::pressed);
}

void CChangeSetGlobalCommit::pressed() {
    emit commitChangeSet();
}
