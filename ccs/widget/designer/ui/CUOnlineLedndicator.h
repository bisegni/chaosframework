#ifndef CDATASETATTRIBUTELEDINDICATOR_H
#define CDATASETATTRIBUTELEDINDICATOR_H

#include "../ChaosBaseDatasetAttributeUI.h"

#include <QIcon>

class CDatasetAttributeLedIndicator:
        public ChaosBaseDatasetAttributeUI {
    Q_OBJECT
public:
    explicit CDatasetAttributeLedIndicator(QWidget *parent = nullptr);
    ~CDatasetAttributeLedIndicator();
public slots:
    void updateOnlineState(OnlineState state);
private slots:
    void updateValue(QVariant new_value);

private:
    OnlineState online_current_state;
    QSharedPointer<QIcon> no_ts;
    QSharedPointer<QIcon> timeouted;
    QSharedPointer<QIcon> alive;
};

#endif // CDATASETATTRIBUTELEDINDICATOR_H
