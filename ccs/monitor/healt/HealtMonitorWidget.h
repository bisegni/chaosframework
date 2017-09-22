#ifndef HEALTMONITORWIDGET_H
#define HEALTMONITORWIDGET_H

#include <QMainWindow>
#include <QMap>
class HealtPresenterWidget;
class HealtWidgetsListPresenteWidget;
class HealtMonitorWidget:
        public QMainWindow {
    Q_OBJECT
public:
    explicit HealtMonitorWidget(QWidget *parent = 0);
protected:
    void closeEvent(QCloseEvent *event) Q_DECL_OVERRIDE;
    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);
protected slots:
    void actionSearchNode();
    void startMonitoringNode(const QString& node_key);
    void stopMonitoringNode(const QString& node_key);
    void closeAllMonitor();
    void selectedNodes(QString tag,
                       QVector<QPair<QString,QString> > selected_nodes);
private:
    QMap<QString, HealtPresenterWidget*> map_node_healt_wdg;
    HealtWidgetsListPresenteWidget *healt_list_presenter;
};

#endif // HEALTMONITORWIDGET_H
