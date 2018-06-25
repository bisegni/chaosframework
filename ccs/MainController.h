#ifndef MAINCONTROLLER_H
#define MAINCONTROLLER_H

#include <QObject>
#include <QApplication>
#include <QSplashScreen>
#include <QSharedPointer>
#include <QMenuBar>
#include "api_async_processor/ApiSubmitter.h"
#include "log_browser/ApplicationErrorLogging.h"

class MainController:
        public QObject,
        public ApiHandler {
    Q_OBJECT
public:
    MainController();
    ~MainController();
    bool init(int argc, const char **argv);
    void deinit();
private slots:
    void selectedNetworkDomain(const QString& selected_domain_network);
    void actionSearchNode();
    void actionPreferences();
    void actionAgentSetting();
    void actionSnaptshotManager();
    void actionTreeGroupManager();
    void actionLogBrowser();
    void actionScriptManager();
    void actionDataService();
    void actionNewUnitServer();
    void actionNewNodeMonitor();
    void actionSwitchNetworkConfiguration();
    void actionApplicationLogBrowser();
    void lastWindowClosed();
    void actionConnectionManager();
    void actionCloseWidget(QObject *widget);
private:
    ApplicationErrorLogging *application_error_widget;
    QMenuBar main_menu_bar;
    ApiSubmitter api_submitter;
    QSharedPointer<QSplashScreen> splash;


    void openInWindow(QWidget *w);
    void initApplicationMenuBar();
    void initConfigurationsMenu(QMenu *menu_configurations);
};

#endif // MAINCONTROLLER_H
