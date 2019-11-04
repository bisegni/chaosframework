#ifndef CHAOSWIDGETS_H
#define CHAOSWIDGETS_H



#include <QtUiPlugin/QDesignerCustomWidgetCollectionInterface>
#include <QtCore/qplugin.h>

class ChaosWidgets:
        public QObject,
        public QDesignerCustomWidgetCollectionInterface {
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QDesignerCustomWidgetCollectionInterface")
    Q_INTERFACES(QDesignerCustomWidgetCollectionInterface)

public:
    ChaosWidgets(QObject *parent = nullptr);

    QList<QDesignerCustomWidgetInterface*> customWidgets() const override;

private:
    QList<QDesignerCustomWidgetInterface*> widgets;
};

#endif // CHAOSWIDGETS_H
