#ifndef CDATASETATTRIBUTESETVALUECOMBOBOXMENUEXTENSION_H
#define CDATASETATTRIBUTESETVALUECOMBOBOXMENUEXTENSION_H

#include <QDesignerTaskMenuExtension>
#include <QExtensionFactory>

QT_BEGIN_NAMESPACE
class QAction;
class QExtensionManager;
QT_END_NAMESPACE
class CDatasetAttributeSetValueComboBox;

class CDatasetAttributeSetValueComboBoxMenuFactory :
        public QExtensionFactory {
    Q_OBJECT
public:
    explicit CDatasetAttributeSetValueComboBoxMenuFactory(QExtensionManager *parent = nullptr);
protected:
    QObject *createExtension(QObject *object, const QString &iid, QObject *parent) const override;
};


class CDatasetAttributeSetValueComboBoxMenuExtension:
        public QObject,
        public QDesignerTaskMenuExtension {
    Q_OBJECT
    Q_INTERFACES(QDesignerTaskMenuExtension)

public:
    explicit CDatasetAttributeSetValueComboBoxMenuExtension(CDatasetAttributeSetValueComboBox *_combo, QObject *parent);

    QAction *preferredEditAction() const override;
    QList<QAction *> taskActions() const override;

private slots:
    void editState();

private:
    QAction *editStateAction;
    CDatasetAttributeSetValueComboBox *combo;
};


#endif // CDATASETATTRIBUTESETVALUECOMBOBOXMENUEXTENSION_H
