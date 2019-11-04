#include "ChaosWidgets.h"

#include <CDatasetAttributeLabelPlugin.h>

ChaosWidgets::ChaosWidgets(QObject *parent):
    QObject(parent) {
    widgets.append(new CDatasetAttributeLabelPlugin(this));
}

QList<QDesignerCustomWidgetInterface*> ChaosWidgets::customWidgets() const {
    return widgets;
}
