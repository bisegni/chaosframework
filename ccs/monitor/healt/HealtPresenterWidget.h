#ifndef HEALTPRESENTERWIDGET_H
#define HEALTPRESENTERWIDGET_H

#include "../handler/healt/healt.h"

#include <QFrame>

namespace Ui {
class HealtPresenterWidget;
}

//! Widget that show the healt information about a node
class HealtPresenterWidget :
        public QFrame
{
    Q_OBJECT

protected:
    //! called when there is a new value for the status


    //! called when there is a new value for the heartbeat

public:
    explicit HealtPresenterWidget(const QString& node_to_check,
                                  QWidget *parent = 0);
    ~HealtPresenterWidget();
public slots:

private:
    Ui::HealtPresenterWidget *ui;
};

#endif // HEALTPRESENTERWIDGET_H
