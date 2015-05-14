#ifndef HEALTMONITORWIDGET_H
#define HEALTMONITORWIDGET_H

#include <QDockWidget>
#include <QVBoxLayout>
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

private:
     QMap<QString, HealtPresenterWidget*> map_node_healt_wdg;
     QVBoxLayout *healt_widget_vlayout;
};

#endif // HEALTMONITORWIDGET_H
