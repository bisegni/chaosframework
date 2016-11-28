#include "HealtMonitorWidget.h"
#include "HealtPresenterWidget.h"
#include "HealtWidgetsListPresenteWidget.h"
#include "../../search/SearchNodeResult.h"

#include <QHBoxLayout>
#include <QPushButton>
#include <QScrollArea>
#include <QHBoxLayout>
#include <QCloseEvent>
#include <QMenuBar>

HealtMonitorWidget::HealtMonitorWidget(QWidget * parent):
    QMainWindow(parent) {
    resize(320, 240);
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowTitle("Node Monitor");
    menuBar()->setNativeMenuBar(false);

    QMenu* menu_tools = menuBar()->addMenu("Tools");
    menu_tools->addAction("Search Node", this, SLOT(actionSearchNode()));

    QWidget *central_widget = new QWidget(this);
    QVBoxLayout *vertical_layout = new QVBoxLayout(central_widget);


    vertical_layout->addWidget(healt_list_presenter = new HealtWidgetsListPresenteWidget(this));
    setCentralWidget(central_widget);
}

void HealtMonitorWidget::closeEvent(QCloseEvent *event) {
    closeAllMonitor();
    event->accept();
}

void HealtMonitorWidget::actionSearchNode() {
    SearchNodeResult *search_node = NULL;
    if((search_node=new SearchNodeResult(true, "select_mon_node"))){
        connect(search_node,
                SIGNAL(selectedNodes(QString,QVector<QPair<QString,QString> >)),
                SLOT(selectedNodes(QString,QVector<QPair<QString,QString> >)));
        search_node->show();
    }
}

void HealtMonitorWidget::selectedNodes(QString tag, QVector<QPair<QString,QString> > selected_nodes) {
    if(tag.compare("select_mon_node") == 0) {
        QVectorIterator<QPair<QString,QString> > it(selected_nodes);
        while(it.hasNext()) {
            startMonitoringNode(it.next().first);
        }
    }
}

void HealtMonitorWidget::startMonitoringNode(const QString& node_key) {
    if(map_node_healt_wdg.contains(node_key)) return;
    HealtPresenterWidget *healt_presenter = new HealtPresenterWidget(node_key);
    map_node_healt_wdg.insert(node_key, healt_presenter);
    healt_list_presenter->addHealtWidget(healt_presenter);
}

void HealtMonitorWidget::stopMonitoringNode(const QString& node_key) {
    if(!map_node_healt_wdg.contains(node_key)) return;
    QMap<QString, HealtPresenterWidget*>::iterator handler_it = map_node_healt_wdg.find(node_key);
    map_node_healt_wdg.erase(handler_it);

    healt_list_presenter->removeHealtWidget(handler_it.value());
    delete(handler_it.value());
}

void HealtMonitorWidget::closeAllMonitor() {
    QList<QString> registered_nodes = map_node_healt_wdg.keys();
    foreach (QString node, registered_nodes) {
        stopMonitoringNode(node);
    }
}
