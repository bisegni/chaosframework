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

    QWidget *widget_container = new QWidget(this);
    QVBoxLayout *parent_layout = new QVBoxLayout(this);

    //add top layout
    QHBoxLayout *hLayout = new QHBoxLayout;
    QPushButton *b1 = new QPushButton("A");
    QPushButton *b2 = new QPushButton("B");
    QPushButton *b3 = new QPushButton("C");
    hLayout->addWidget(b1);
    hLayout->addWidget(b2);
    hLayout->addWidget(b3);

    QScrollArea *healt_widget_scroll_area = new QScrollArea(this);
    QWidget *scroll_content = new QWidget(healt_widget_scroll_area);

    healt_widget_vlayout = new QVBoxLayout(this);
    scroll_content->setLayout(healt_widget_vlayout);
    healt_widget_scroll_area->setWidget(scroll_content);

    // add bottom layout
    QHBoxLayout *hLayout_bottom = new QHBoxLayout();
    QPushButton *bb1 = new QPushButton("A");
    QPushButton *bb2 = new QPushButton("B");
    QPushButton *bb3 = new QPushButton("C");
    hLayout_bottom->addWidget(bb1);
    hLayout_bottom->addWidget(bb2);
    hLayout_bottom->addWidget(bb3);


    parent_layout->addLayout(hLayout);
    parent_layout->addWidget(healt_widget_scroll_area);
    parent_layout->addLayout(hLayout_bottom);
    widget_container->setLayout(parent_layout);
    setWidget(widget_container);

    //register this widget for global monitoring signals
//    connect(MainController::getInstance(),
//            SIGNAL(startMonitoringNodeForHealt(QString)),
//            SLOT(startMonitoringNode(QString)));
//    connect(MainController::getInstance(),
//            SIGNAL(stopMonitoringNodeForHealt(QString)),
//            SLOT(stopMonitoringNode(QString)));
}

void HealtMonitorWidget::startMonitoringNode(const QString& node_key) {
    if(map_node_healt_wdg.contains(node_key)) return;
    HealtPresenterWidget *healt_presenter = new HealtPresenterWidget(node_key);

    map_node_healt_wdg.insert(node_key, healt_presenter);
    healt_widget_vlayout->addWidget(healt_presenter);
}

void HealtMonitorWidget::stopMonitoringNode(const QString& node_key) {
    if(!map_node_healt_wdg.contains(node_key)) return;
    QMap<QString, HealtPresenterWidget*>::iterator handler = map_node_healt_wdg.find(node_key);
    map_node_healt_wdg.erase(handler);
    healt_widget_vlayout->removeWidget(handler.value());
    delete(handler.value());
}
