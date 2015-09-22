#ifndef CHAOSHEALTLABEL_H
#define CHAOSHEALTLABEL_H

#include "ChaosLabel.h"

class ChaosHealtLabel:
        public ChaosLabel {
public:

    ChaosHealtLabel(QWidget *parent = 0);
    ~ChaosHealtLabel();
    int startMonitoring();
    int stopMonitoring();
signals:

public slots:
private:
    boost::shared_ptr<AbstractAttributeHandler> handler_sptr;
    boost::shared_ptr<AbstractAttributeHandler>
    getAttributeHandlerForType(ChaosDataType chaos_type, bool &ok);
};

#endif // CHAOSHEALTLABEL_H
