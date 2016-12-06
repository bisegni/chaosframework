#include "CQListView.h"

CQListView::CQListView(QWidget *parent):
    QListView(parent) {}

void CQListView::setResizeOnModelReset(bool new_resize_on_reset) {
    resize_on_reset = new_resize_on_reset;
}

bool CQListView::resizeOnModelReset() {
    return resize_on_reset;
}

void CQListView::setModel(QAbstractItemModel *model) {
    if(QListView::model()) {
        disconnect(QListView::model(),
                   SIGNAL(modelReset()),
                   this,SLOT(modelResetEvent()));
    }
    QListView::setModel(model);
    if(model) {
        connection = connect(model,
                             SIGNAL(modelReset()),
                             SLOT(modelResetEvent()));
    }
}

void CQListView::modelResetEvent() {
    setMinimumHeight(0);
    setMaximumWidth(sizeHintForColumn(0)+5);
}
