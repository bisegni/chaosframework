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
signals:

private slots:
    void modelResetEvent();
private:
    bool resize_on_reset;
    QMetaObject::Connection connection;
};

#endif // CQLISTVIEW_H
