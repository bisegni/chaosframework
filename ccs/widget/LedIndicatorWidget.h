#ifndef LEDINDICATORWIDGET_H
#define LEDINDICATORWIDGET_H

#include <QMap>
#include <QIcon>
#include <QWidget>
#include <QSharedPointer>

class LedIndicatorWidget :
        public QWidget
{
    Q_OBJECT

public:
    explicit LedIndicatorWidget(QWidget *parent = 0);
    void addState(int state_value,
                  QSharedPointer<QIcon> state_icon);
    int getState();
    void paintEvent ( QPaintEvent *  );
signals:

public slots:
    void setState(int state);

private:
    int current_state;
    QMap<int, QSharedPointer<QIcon> > map_state_icon;
};

#endif // LEDINDICATORWIDGET_H
