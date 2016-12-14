#ifndef MAINCONTROLLER_H
#define MAINCONTROLLER_H

#include <QObject>
#include <QApplication>
#include <QSplashScreen>
#include <QSharedPointer>
#include <QMenuBar>
#include "api_async_processor/ApiSubmitter.h"

class MainController:
public QObject,
public ApiHandler {
    Q_OBJECT
public:
    MainController();
    ~MainController();
    void init(int argc, char **argv, QApplication &a);
    void deinit();
private slots:
    void actionSearchNode();
    void actionPreferences();
    void actionSnaptshotManager();
    void actionTreeGroupManager();
    void actionLogBrowser();
    void actionScriptManager();
    void actionDataService();
    void actionNewUnitServer();
    void actionNewNodeMonitor();
    void actionSwitchNetworkConfiguration();
    void lastWindowClosed();
private:
    QMenuBar main_menu_bar;
    ApiSubmitter api_submitter;
    QSharedPointer<QSplashScreen> splash;

    bool reconfigure();
    void openInWindow(QWidget *w);
    void initApplicationMenuBar();
    void initConfigurationsMenu(QMenu *menu_configurations);
};

#endif // MAINCONTROLLER_H
