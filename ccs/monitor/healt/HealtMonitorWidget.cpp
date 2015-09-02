#include "HealtMonitorWidget.h"
#include "HealtPresenterWidget.h"


#include <QHBoxLayout>
#include <QPushButton>
#include <QScrollArea>

HealtMonitorWidget::HealtMonitorWidget(QWidget * parent):
    QDockWidget(tr("Healt Monitor"), parent) {
    setStyleSheet("::title { position: relative; text-align: right }");
    setMinimumWidth(240);

    setFeatures(QDockWidget::DockWidgetMovable|
                QDockWidget::DockWidgetFloatable);

    setWidget(healt_list_presenter = new HealtWidgetsListPresenteWidget(this));
}

void HealtMonitorWidget::startMonitoringNode(const QString& node_key) {
    if(map_node_healt_wdg.contains(node_key)) return;
    HealtPresenterWidget *healt_presenter = new HealtPresenterWidget(node_key);

    map_node_healt_wdg.insert(node_key, healt_presenter);
    healt_list_presenter->addHealtWidget(healt_presenter);

    //show if necessary the monitor widget
    if(isHidden()) show();
}

void HealtMonitorWidget::stopMonitoringNode(const QString& node_key) {
    if(!map_node_healt_wdg.contains(node_key)) return;
    QMap<QString, HealtPresenterWidget*>::iterator handler = map_node_healt_wdg.find(node_key);
    map_node_healt_wdg.erase(handler);

    healt_list_presenter->removeHealtWidget(handler.value());
    delete(handler.value());
}
