#include "ChaosWidgets.h"

//ui
#include "CUOnlineLedIndicatorPlugin.h"
#include "CDatasetAttributeLabelPlugin.h"
#include "CDatasetAttributeImagePlugin.h"
#include "CDatasetAttributeHealthLabelPlugin.h"
#include "CDatasetAttributeSet2VButtonPlugin.h"
#include "CDatasetAttributeSetValueLineEditPlugin.h"
#include "CDatasetAttributeSetValueComboBoxPlugin.h"
//command
#include "CChangeSetGlobalCommitPlugin.h"

ChaosWidgets::ChaosWidgets(QObject *parent):
    QObject(parent) {
    widgets.append(new CUOnlineLedIndicatorPlugin(this));
    widgets.append(new CDatasetAttributeLabelPlugin(this));
    widgets.append(new CDatasetAttributeImagePlugin(this));
    widgets.append(new CDatasetAttributeHealthLabelPlugin(this));
    widgets.append(new CDatasetAttributeSet2VButtonPlugin(this));
    widgets.append(new CDatasetAttributeSetValueLineEditPlugin(this));
    widgets.append(new CDatasetAttributeSetValueComboBoxPlugin(this));

    widgets.append(new CChangeSetGlobalCommitPlugin(this));
}

QList<QDesignerCustomWidgetInterface*> ChaosWidgets::customWidgets() const {
    return widgets;
}
