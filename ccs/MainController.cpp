#include "MainController.h"
#include "data/data.h"
#include "data/delegate/TwoLineInformationItem.h"
#include "data/ChaosByteArray.h"
#include <QStyleFactory>
#include <QThreadPool>
#include <QTimer>
#include <QTextStream>
#include <QDebug>
#include <QFile>
#include <QString>
#include <string>
#include <QInputDialog>
#include <QTextStream>
#include <QFile>
#include <QMenu>

#include <ChaosMetadataServiceClient/ChaosMetadataServiceClient.h>

#include "node/unit_server/UnitServerEditor.h"
#include "node/data_service/DataServiceEditor.h"
#include "search/SearchNodeResult.h"
#include "preference/PreferenceDialog.h"
#include "preference/PreferenceManager.h"
#include "snapshot/SnapshotManager.h"
#include "tree_group/TreeGroupManager.h"
#include "log_browser/LogBrowser.h"
#include "script/ScriptManager.h"
#include "monitor/healt/HealtMonitorWidget.h"
#include "GlobalServices.h"

#include "metatypes.h"

using namespace chaos::metadata_service_client;

//declare metatype used in chaos
MainController::MainController():
    api_submitter(this){}

MainController::~MainController() {}

void MainController::init(int argc, char **argv, QApplication& a) {
    //register chaos metatype
    qRegisterMetaType<QSharedPointer<ChaosByteArray> >();
    qRegisterMetaType<QSharedPointer<CommandReader> >();
    qRegisterMetaType<QSharedPointer<CommandParameterReader> >();
    qRegisterMetaType<QSharedPointer<DatasetReader> >();
    qRegisterMetaType<QSharedPointer<DatasetAttributeReader> >();
    qRegisterMetaType<QSharedPointer<chaos::CException> >();
    qRegisterMetaType<QSharedPointer<chaos::common::data::CDataWrapper> >("QSharedPointer<CDataWrapper>");
    qRegisterMetaType<QSharedPointer<chaos::common::data::CDataWrapper> >("QSharedPointer<chaos::common::data::CDataWrapper>");
    qRegisterMetaType<QSharedPointer<TwoLineInformationItem> >();
    qRegisterMetaType<QSharedPointer<chaos::metadata_service_client::api_proxy::node::CommandTemplate> >();
    qRegisterMetaType<boost::shared_ptr<chaos::metadata_service_client::api_proxy::node::CommandTemplate> >();
    qRegisterMetaType<std::string>("std::string");
    qRegisterMetaType<int32_t>("int32_t");
    qRegisterMetaType<int64_t>("int64_t");
    qRegisterMetaType<uint64_t>("uint64_t");
    qRegisterMetaType<boost::shared_ptr<chaos::common::data::SerializationBuffer> >();
    qRegisterMetaType<boost::shared_ptr<chaos::common::data::SerializationBuffer> >("boost::shared_ptr<chaos::common::data::SerializationBuffer>&");
    qRegisterMetaType<boost::shared_ptr<chaos::common::data::CDataWrapper> >("boost::shared_ptr<chaos::common::data::CDataWrapper>");
    qRegisterMetaType<boost::shared_ptr<chaos::CException> >("chaos::CException");
    qRegisterMetaType<boost::shared_ptr<chaos::common::data::CDataWrapper> >("chaos::metadata_service_client::monitor_system::KeyValue");
    qRegisterMetaType<chaos::metadata_service_client::node_monitor::OnlineState>("chaos::metadata_service_client::node_monitor::OnlineState");
    qRegisterMetaType<chaos::service_common::data::node::NodeInstance>("chaos::service_common::data::node::NodeInstance");
    qRegisterMetaType<chaos::common::data::CDataVariant>("chaos::common::data::CDataVariant");
    qRegisterMetaType<QSharedPointer<TwoLineInformationItem> >("QSharedPointer<TwoLineInformationItem>");

    a.setStyle(QStyleFactory::create("Fusion"));
    QColor dark_main(95,95,95);
    QColor dark_main_desktop(43,43,43);
    QColor default_text(220,220,220);
    QColor disable_color(155,155,155);

    QPalette darkPalette;
    darkPalette.setColor(QPalette::Window, dark_main);
    darkPalette.setColor(QPalette::WindowText, default_text);
    darkPalette.setColor(QPalette::Base, QColor(25,25,25));
    darkPalette.setColor(QPalette::AlternateBase, dark_main);
    darkPalette.setColor(QPalette::ToolTipBase, Qt::white);
    darkPalette.setColor(QPalette::ToolTipText, Qt::white);
    darkPalette.setColor(QPalette::Text, default_text);
    darkPalette.setColor(QPalette::Button, dark_main);
    darkPalette.setColor(QPalette::ButtonText, Qt::white);
    darkPalette.setColor(QPalette::BrightText, Qt::red);
    darkPalette.setColor(QPalette::Link, QColor(42, 130, 218));

    darkPalette.setColor(QPalette::Highlight, QColor(42, 130, 218));
    darkPalette.setColor(QPalette::HighlightedText, Qt::black);
    darkPalette.setColor(QPalette::Disabled, QPalette::ButtonText, disable_color);
    darkPalette.setColor(QPalette::Disabled, QPalette::Text, disable_color);

    a.setPalette(darkPalette);

#ifdef Q_OS_LINUX
    a.setStyleSheet("QWidget {font-family: Monospace; font-size: 9pt;}"
                    "QToolTip { color: #ffffff; background-color: #2a82da; border: 1px solid white; }");
#elif defined(Q_OS_DARWIN)
    //    QFile style_files(":/dark_orange/style.qss");
    //    if (!style_files.open(QFile::ReadOnly | QFile::Text)) {
    a.setStyleSheet("QWidget {font-size: 11pt;}"
                    "QToolTip { color: #ffffff; background-color: #2a82da; border: 1px solid white; }");
    //    }else{
    //        QTextStream in(&style_files);
    //        a.setStyleSheet(in.readAll());
    //    }
#else

#endif
    //set application information
    a.setQuitOnLastWindowClosed(false);
    QApplication::setApplicationName("chaos_control_studio");
    QApplication::setApplicationVersion("0.0.1-alpha");
    QApplication::setOrganizationName("INFN-LNF");
    QApplication::setOrganizationDomain("chaos.infn.it");

    //initialize !CHAOS metadata service client
    ChaosMetadataServiceClient::getInstance()->init(argc, argv);
    ChaosMetadataServiceClient::getInstance()->start();

    //init menu bar
    initApplicationMenuBar();

    //configure monitor
    bool configured = reconfigure();

    //show splash screen
    QPixmap pixmap(":splash/main_splash.png");
    splash.reset(new QSplashScreen(pixmap));
    splash->show();
    splash->showMessage(QObject::tr("Starting !CHAOS layer..."),
                        Qt::AlignLeft | Qt::AlignBottom, Qt::lightGray);
    a.processEvents();
    //keep segnal for last windows closed
    connect(&a,
            SIGNAL(lastWindowClosed()),
            SLOT(lastWindowClosed()));

    splash->showMessage(QObject::tr("!CHAOS Control Studio Initilized!"),
                        Qt::AlignLeft | Qt::AlignBottom, Qt::lightGray);
    a.processEvents();
    //set thread pool thread size
    qDebug() << "Thread pool of size:" << QThreadPool::globalInstance()->maxThreadCount();


    if(configured == false) {
        splash->finish(NULL);
        actionPreferences();
    }
    QWidget *w = new SearchNodeResult();
    w->show();
    splash->finish(w);
}

void MainController::deinit() {
    //clear the qthread pool
    QThreadPool::globalInstance()->clear();
    QThreadPool::globalInstance()->waitForDone();

    //! deinitialize !CHOAS  metadata service client
    ChaosMetadataServiceClient::getInstance()->stop();
    ChaosMetadataServiceClient::getInstance()->deinit();

    qDebug() << "!CHAOS Control Studio closed!";
}


bool MainController::reconfigure() {
    bool configured = true;
    try{
        configured = PreferenceManager::getInstance()->activerNetworkConfiguration(PreferenceManager::getInstance()->getActiveConfigurationName());
    }catch(...) {
        configured = false;
    }
    return configured;
}

void MainController::initApplicationMenuBar() {
    main_menu_bar.setNativeMenuBar(true);
    //node menu
    QMenu *menu = main_menu_bar.addMenu("&Node");
    menu->addAction("New Unit Server", this, SLOT(actionNewUnitServer()));
    menu->addAction("Search Node", this, SLOT(actionSearchNode()),QKeySequence(Qt::CTRL + Qt::Key_N));
    menu->addAction("Data Service Editor", this, SLOT(actionDataService()),QKeySequence(Qt::CTRL + Qt::Key_D));

    //Data
    menu = main_menu_bar.addMenu("&Data");
    menu->addAction("Snapshot manager", this, SLOT(actionSnaptshotManager()),QKeySequence(Qt::CTRL + Qt::Key_S));
    menu->addAction("Group manager", this, SLOT(actionTreeGroupManager()),QKeySequence(Qt::CTRL + Qt::Key_G));
    menu->addAction("Log Browser", this, SLOT(actionLogBrowser()),QKeySequence(Qt::CTRL + Qt::Key_L));

    //Data
    menu = main_menu_bar.addMenu("&Algorithm");
    menu->addAction("Script Manager", this, SLOT(actionScriptManager()),QKeySequence(Qt::CTRL + Qt::Key_P));

    //Data
    menu = main_menu_bar.addMenu("&Tools");
    menu->addAction("Node Monitor", this, SLOT(actionNewNodeMonitor()),QKeySequence(Qt::CTRL + Qt::Key_P));
    menu->addSeparator();
    initConfigurationsMenu(menu->addMenu("Network Configurations"));
    menu->addAction("Preferenes", this, SLOT(actionPreferences()));
    main_menu_bar.show();
}

void MainController::initConfigurationsMenu(QMenu *menu_configurations) {
    QAction *action_configuration = NULL;
    QActionGroup* group_configuration = new QActionGroup(this);
    QStringList networ_configurations = PreferenceManager::getInstance()->getNetowrkConfigurationsNames();
    const QString current_setting = PreferenceManager::getInstance()->getActiveConfigurationName();
    foreach (QString configuration, networ_configurations) {
        action_configuration = menu_configurations->addAction(configuration, this, SLOT(actionSwitchNetworkConfiguration()));
        action_configuration->setCheckable(true);
        action_configuration->setChecked(current_setting.compare(configuration) == 0);
        action_configuration->setActionGroup(group_configuration);
    }
}

void MainController::openInWindow(QWidget *w) {
    if(w == NULL) return;
    w->show();
}

void MainController::lastWindowClosed() {

}

void MainController::actionSearchNode() {
    openInWindow(new SearchNodeResult());
}

void MainController::actionSnaptshotManager() {
    openInWindow(new SnapshotManager());
}

void MainController::actionTreeGroupManager() {
    openInWindow(new TreeGroupManager());
}

void MainController::actionLogBrowser() {
    openInWindow(new LogBrowser());
}

void MainController::actionScriptManager() {
    openInWindow(new ScriptManager());
}

void MainController::actionDataService() {
    openInWindow(new DataServiceEditor());
}

void MainController::actionNewNodeMonitor() {
    openInWindow(new HealtMonitorWidget());
}

void MainController::actionSwitchNetworkConfiguration() {
    QAction* action_network_configuration = dynamic_cast<QAction*>(sender());
    if(action_network_configuration == NULL) return;
    PreferenceManager::getInstance()->activerNetworkConfiguration(action_network_configuration->text());
}

void MainController::actionNewUnitServer() {
    bool ok = false;
    QString unit_server_uid = QInputDialog::getText(NULL,
                                                    tr("Create new control unit type"),
                                                    tr("Control Unit Type:"),
                                                    QLineEdit::Normal,
                                                    tr(""), &ok);
    if(ok && unit_server_uid.size() > 0) {
        api_submitter.submitApiResult("new_unit_server",
                                      GET_CHAOS_API_PTR(chaos::metadata_service_client::api_proxy::unit_server::NewUS)->execute(unit_server_uid.toStdString()));
    }
}

void MainController::actionPreferences() {
    PreferenceDialog pref_dialog(NULL);
    connect(&pref_dialog,
            SIGNAL(changedConfiguration()),
            SLOT(reconfigure()));
    pref_dialog.exec();
}
