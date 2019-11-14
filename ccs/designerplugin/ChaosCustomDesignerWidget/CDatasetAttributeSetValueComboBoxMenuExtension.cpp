#include "CDatasetAttributeSetValueComboBoxMenuExtension.h"
#include "CDatasetAttributeSetValueComboBoxDialog.h"

#include "../../widget/designer/ui/CDatasetAttributeSetValueComboBox.h"
#include <QtDesigner/QDesignerFormEditorInterface>
#include <QAction>

CDatasetAttributeSetValueComboBoxMenuFactory::CDatasetAttributeSetValueComboBoxMenuFactory(QExtensionManager *parent):
    QExtensionFactory(parent){}

QObject *CDatasetAttributeSetValueComboBoxMenuFactory::createExtension(QObject *object,
                                                                       const QString &iid, QObject *parent) const {
    if (iid != Q_TYPEID(QDesignerTaskMenuExtension))
        return nullptr;

    if (CDatasetAttributeSetValueComboBox *widget = qobject_cast<CDatasetAttributeSetValueComboBox*>(object))
        return new CDatasetAttributeSetValueComboBoxMenuExtension(widget, parent);

    return nullptr;
}

CDatasetAttributeSetValueComboBoxMenuExtension::CDatasetAttributeSetValueComboBoxMenuExtension(CDatasetAttributeSetValueComboBox *_combo, QObject *parent)
    : QObject(parent)
    , editStateAction(new QAction(tr("Edit State..."), this))
    , combo(_combo) {
    connect(editStateAction, &QAction::triggered, this, &CDatasetAttributeSetValueComboBoxMenuExtension::editState);
}

void CDatasetAttributeSetValueComboBoxMenuExtension::editState() {
    CDatasetAttributeSetValueComboBoxDialog dialog(combo);
    dialog.exec();
}

QAction *CDatasetAttributeSetValueComboBoxMenuExtension::preferredEditAction() const {
    return editStateAction;
}

QList<QAction *> CDatasetAttributeSetValueComboBoxMenuExtension::taskActions() const {
    return QList<QAction *>{editStateAction};
}

