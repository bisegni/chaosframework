#ifndef CQLISTVIEW_H
#define CQLISTVIEW_H

#include <QListView>

class CQListView :
        public QListView {
    Q_OBJECT
public:
    explicit CQListView(QWidget *parent = 0);
    void setResizeOnModelReset(bool new_resize_on_reset);
    bool resizeOnModelReset();
    void setModel(QAbstractItemModel *model);
protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
signals:

private slots:
    void modelResetEvent();
private:
    bool resize_on_reset;
    QPoint drag_start_position;
    QMetaObject::Connection connection;
};

#endif // CQLISTVIEW_H
