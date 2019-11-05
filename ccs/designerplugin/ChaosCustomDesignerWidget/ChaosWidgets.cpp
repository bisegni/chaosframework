#include "ChaosWidgets.h"

#include <CDatasetAttributeLabelPlugin.h>
#include <CUOnlineLedIndicatorPlugin.h>

ChaosWidgets::ChaosWidgets(QObject *parent):
    QObject(parent) {
    widgets.append(new CDatasetAttributeLabelPlugin(this));
    widgets.append(new CUOnlineLedIndicatorPlugin(this));
}

QList<QDesignerCustomWidgetInterface*> ChaosWidgets::customWidgets() const {
    return widgets;
}
