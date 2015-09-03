#ifndef HEALTMONITORWIDGET_H
#define HEALTMONITORWIDGET_H

#include "../handler/handler.h"
#include "HealtWidgetsListPresenteWidget.h"

#include <QDockWidget>
#include <QScrollArea>
#include <QMap>

#include <ChaosMetadataServiceClient/ChaosMetadataServiceClient.h>

class HealtPresenterWidget;

class HealtMonitorWidget :
        public QDockWidget
{
    Q_OBJECT
public:
    explicit HealtMonitorWidget(QWidget *parent = 0);

public slots:
    void startMonitoringNode(const QString& node_key);
    void stopMonitoringNode(const QString& node_key);
    void closeAllMonitor();
private:
     QMap<QString, HealtPresenterWidget*> map_node_healt_wdg;
     HealtWidgetsListPresenteWidget *healt_list_presenter;
};

#endif // HEALTMONITORWIDGET_H
