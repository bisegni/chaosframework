#ifndef CUOnlineLedIndicator_H
#define CUOnlineLedIndicator_H

#include "../ChaosBaseDatasetAttributeUI.h"

#include <QIcon>
#include <QSharedPointer>
#include <QMutex>

struct CUOnlineLedIndicatorStateInfo {
    bool blink_on_repeat_set;
    QSharedPointer<QIcon> icon;
    CUOnlineLedIndicatorStateInfo( QSharedPointer<QIcon> _icon,
               bool _blink_on_repeat_set):
        blink_on_repeat_set(_blink_on_repeat_set),
        icon(_icon){}
};

class CUOnlineLedIndicator:
        public ChaosBaseDatasetAttributeUI {
    Q_OBJECT
public:
    explicit CUOnlineLedIndicator(QWidget *parent = nullptr);
    virtual ~CUOnlineLedIndicator();
    void addState(int state_value,
                  QSharedPointer<QIcon> state_icon,
                  bool blonk_on_repeat_set = false);
    int getState();
    void setState(int state);
    QSize minimumSizeHint() const override;
    QSize sizeHint() const override;
protected:
    void paintEvent(QPaintEvent *event) override;
    void updateOnline(ChaosBaseDatasetUI::OnlineState state) override;
    void updateValue(QVariant variant_value) override;

private:
    OnlineState online_current_state;
    int current_state;
    int blink_counter;
    mutable QMutex map_mutex;
    QMap<int, QSharedPointer<CUOnlineLedIndicatorStateInfo> > map_state_info;
    QSharedPointer<QIcon> no_ts;
    QSharedPointer<QIcon> timeouted;
    QSharedPointer<QIcon> alive;
};

#endif // CUOnlineLedIndicator_H
