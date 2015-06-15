#ifndef LEDINDICATORWIDGET_H
#define LEDINDICATORWIDGET_H

#include <QMap>
#include <QIcon>
#include <QWidget>
#include <QTimer>
#include <QSharedPointer>

struct StateInfo {
    bool blink_on_repeat_set;
    QSharedPointer<QIcon> icon;
    StateInfo( QSharedPointer<QIcon> _icon,
               bool _blink_on_repeat_set):
    icon(_icon),
    blink_on_repeat_set(_blink_on_repeat_set){}
};

class LedIndicatorWidget :
        public QWidget
{
    Q_OBJECT

public:
    explicit LedIndicatorWidget(QWidget *parent = 0);
    void addState(int state_value,
                  QSharedPointer<QIcon> state_icon,
                  bool blonk_on_repeat_set = false);
    int getState();
    void blink();
    void paintEvent ( QPaintEvent *  );
signals:

public slots:
    void setState(int state);
    void setStateBlinkOnRepeatSet(int state,
                                  bool blonk_on_repeat_set);
private slots:
private slots:
    void tick();

private:
    int blink_counter;
    int current_state;
    QTimer blink_timer;

    mutable QMutex map_mutex;
    QMap<int, QSharedPointer<StateInfo> > map_state_info;
    //a state can be configured to blink when set method is called with
    //the same state

};

#endif // LEDINDICATORWIDGET_H
