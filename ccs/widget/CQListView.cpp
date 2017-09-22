#include "CQListView.h"
#include <QMouseEvent>
#include <QApplication>
#include <QDrag>
#include <QMimeData>
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
    //  setwi(sizeHintForColumn(0)+5);
}

void CQListView::mousePressEvent(QMouseEvent *event) {
    QListView::mousePressEvent(event);
    if (event->button() == Qt::LeftButton) {
        drag_start_position = event->pos();
    }
}

void CQListView::mouseMoveEvent(QMouseEvent *event) {
    if (!(event->buttons() & Qt::LeftButton))
        return;
    if ((event->pos() - drag_start_position).manhattanLength()
            < QApplication::startDragDistance())
        return;

    QDrag *drag = new QDrag(this);
    QMimeData *mimeData = new QMimeData;

    mimeData->setText("text");
    drag->setMimeData(mimeData);

    Qt::DropAction dropAction = drag->exec(Qt::CopyAction | Qt::MoveAction);

}
