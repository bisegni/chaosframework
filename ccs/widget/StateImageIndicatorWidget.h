#ifndef StateImageIndicatorWidget_H
#define StateImageIndicatorWidget_H

#include <QMap>
#include <QIcon>
#include <QWidget>
#include <QTimer>
#include <QSharedPointer>
#include <QMutex>

struct StateInfo {
    bool blink_on_repeat_set;
    QSharedPointer<QIcon> icon;
    StateInfo( QSharedPointer<QIcon> _icon,
               bool _blink_on_repeat_set):
        blink_on_repeat_set(_blink_on_repeat_set),
        icon(_icon){}
};

class StateImageIndicatorWidget :
        public QWidget {
    Q_OBJECT

public:
    explicit StateImageIndicatorWidget(QWidget *parent = nullptr);
    void addState(int state_value,
                  QSharedPointer<QIcon> state_icon,
                  bool blonk_on_repeat_set = false);
    int getState();
    void blink();
protected:
    void paintEvent ( QPaintEvent * event ) override;

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

#endif // StateImageIndicatorWidget_H
