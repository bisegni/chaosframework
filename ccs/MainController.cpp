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
#include <QResource>
#include <QWindow>

#include "error/ErrorManager.h"
#include "node/unit_server/UnitServerEditor.h"
#include "node/data_service/DataServiceEditor.h"
#include "search/SearchNodeResult.h"
#include "preference/PreferenceDialog.h"
#include "preference/PreferenceManager.h"
#include "preference/SelectNetworkDomain.h"
#include "snapshot/SnapshotManager.h"
#include "tree_group/TreeGroupManager.h"
#include "log_browser/LogBrowser.h"
#include "script/ScriptManager.h"
#include "monitor/healt/HealtMonitorWidget.h"
#include "node/agent/AgentSetting.h"
#include "node/connection_manager/ConnectionEditor.h"
#include "GlobalServices.h"

#include "metatypes.h"

using namespace chaos::metadata_service_client;

//declare metatype used in chaos
MainController::MainController():
    application_error_widget(NULL),
    api_submitter(this) {
    connect(&ErrorManager::getInstance()->signal_proxy,
            SIGNAL(errorEntryUpdated()),
            SLOT(actionApplicationLogBrowser()));
}

MainController::~MainController() {}

bool MainController::init(int argc, const char **argv) {
    //set application information
    ((QApplication*)QApplication::instance())->setQuitOnLastWindowClosed(false);
    QApplication::setApplicationName("ChaosControlStudio");
    QApplication::setApplicationVersion("1.0.0-alpha");
    QApplication::setOrganizationName("INFN-LNF");
    QApplication::setOrganizationDomain("chaos.infn.it");

    //keep segnal for last windows closed
    connect(QApplication::instance(),
            SIGNAL(lastWindowClosed()),
            SLOT(lastWindowClosed()));

    //show splash screen
    QPixmap pixmap(":splash/main_splash.png");
    splash.reset(new QSplashScreen(pixmap));
    splash->show();
    splash->showMessage(QObject::tr("Starting !CHAOS layer..."),
                        Qt::AlignLeft | Qt::AlignBottom, Qt::lightGray);

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
    qRegisterMetaType<ChaosSharedPtr<chaos::metadata_service_client::api_proxy::node::CommandTemplate> >();
    qRegisterMetaType<std::string>("std::string");
    qRegisterMetaType<int32_t>("int32_t");
    qRegisterMetaType<int64_t>("int64_t");
    qRegisterMetaType<uint64_t>("uint64_t");
    qRegisterMetaType<ChaosSharedPtr<chaos::common::data::SerializationBuffer> >();
    qRegisterMetaType<ChaosSharedPtr<chaos::common::data::SerializationBuffer> >("ChaosSharedPtr<chaos::common::data::SerializationBuffer>&");
    qRegisterMetaType<ChaosSharedPtr<chaos::common::data::CDataWrapper> >("ChaosSharedPtr<chaos::common::data::CDataWrapper>");
    qRegisterMetaType<ChaosSharedPtr<chaos::CException> >("chaos::CException");
    qRegisterMetaType<ChaosSharedPtr<chaos::common::data::CDataWrapper> >("chaos::metadata_service_client::monitor_system::KeyValue");
    qRegisterMetaType<chaos::metadata_service_client::node_monitor::OnlineState>("chaos::metadata_service_client::node_monitor::OnlineState");
    qRegisterMetaType<chaos::service_common::data::node::NodeInstance>("chaos::service_common::data::node::NodeInstance");
    qRegisterMetaType<chaos::service_common::data::script::ScriptInstance>("chaos::service_common::data::script::ScriptInstance");
    qRegisterMetaType<chaos::common::data::CDataVariant>("chaos::common::data::CDataVariant");
    qRegisterMetaType<QSharedPointer<TwoLineInformationItem> >("QSharedPointer<TwoLineInformationItem>");

    //settin application style
    qApp->setStyle(QStyleFactory::create("Fusion"));
    if(true) {
        // modify palette to dark
        QPalette darkPalette;
        darkPalette.setColor(QPalette::Window,QColor(53,53,53));
        darkPalette.setColor(QPalette::WindowText,Qt::white);
        darkPalette.setColor(QPalette::Disabled,QPalette::WindowText,QColor(127,127,127));
        darkPalette.setColor(QPalette::Base,QColor(42,42,42));
        darkPalette.setColor(QPalette::AlternateBase,QColor(66,66,66));
        darkPalette.setColor(QPalette::ToolTipBase,Qt::white);
        darkPalette.setColor(QPalette::ToolTipText,Qt::white);
        darkPalette.setColor(QPalette::Text,Qt::white);
        darkPalette.setColor(QPalette::Disabled,QPalette::Text,QColor(127,127,127));
        darkPalette.setColor(QPalette::Dark,QColor(35,35,35));
        darkPalette.setColor(QPalette::Shadow,QColor(20,20,20));
        darkPalette.setColor(QPalette::Button,QColor(53,53,53));
        darkPalette.setColor(QPalette::ButtonText,Qt::white);
        darkPalette.setColor(QPalette::Disabled,QPalette::ButtonText,QColor(127,127,127));
        darkPalette.setColor(QPalette::BrightText,Qt::red);
        darkPalette.setColor(QPalette::Link,QColor(42,130,218));
        darkPalette.setColor(QPalette::Highlight,QColor(42,130,218));
        darkPalette.setColor(QPalette::Disabled,QPalette::Highlight,QColor(80,80,80));
        darkPalette.setColor(QPalette::HighlightedText,Qt::white);
        darkPalette.setColor(QPalette::Disabled,QPalette::HighlightedText,QColor(127,127,127));

        qApp->setPalette(darkPalette);
    } else {
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

        ((QApplication*)QApplication::instance())->setPalette(darkPalette);
    }

#ifdef Q_OS_LINUX
    ((QApplication*)QApplication::instance())->setStyleSheet("QWidget {font-family: Monospace; font-size: 9pt;}"
                                                             "QToolTip { color: #ffffff; background-color: #2a82da; border: 1px solid white; }");
#elif defined(Q_OS_DARWIN)
    //    QFile style_files(":/dark_orange/style.qss");
    //    if (!style_files.open(QFile::ReadOnly | QFile::Text)) {
    ((QApplication*)QApplication::instance())->setStyleSheet("QWidget {font-size: 11pt;}"
                                                             "QToolTip { color: #ffffff; background-color: #2a82da; border: 1px solid white; }");
    //    }else{
    //        QTextStream in(&style_files);
    //        QApplication::instance()->setStyleSheet(in.readAll());
    //    }
#else

#endif
    QApplication::instance()->processEvents();
    //initialize !CHAOS metadata service client
    ChaosMetadataServiceClient::getInstance()->init(argc, argv);
    ChaosMetadataServiceClient::getInstance()->start();
    QApplication::instance()->processEvents();

    splash->showMessage(QObject::tr("!CHAOS Control Studio Initilized!"),
                        Qt::AlignLeft | Qt::AlignBottom, Qt::lightGray);
    QApplication::instance()->processEvents();
    //set thread pool thread size
    qDebug() << "Thread pool of size:" << QThreadPool::globalInstance()->maxThreadCount();
    splash->close();
    //start selection of the network domain
    SelectNetworkDomain network_dmoain_selector;
    connect(&network_dmoain_selector,
            SIGNAL(networkDomainSelected(QString)),
            SLOT(selectedNetworkDomain(QString)));
    bool result = (network_dmoain_selector.exec() == QDialog::Accepted);
    return result;
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

void MainController::initApplicationMenuBar() {
    main_menu_bar.setNativeMenuBar(true);
    main_menu_bar.clear();
    //node menu
    QMenu *menu = main_menu_bar.addMenu("&Node");
    menu->addAction("New Unit Server", this, SLOT(actionNewUnitServer()));
    menu->addAction("Search Node", this, SLOT(actionSearchNode()),QKeySequence(Qt::CTRL + Qt::Key_N));
    menu->addAction("Data Service Editor", this, SLOT(actionDataService()),QKeySequence(Qt::CTRL + Qt::Key_D));

    //Data
    menu = main_menu_bar.addMenu("&Data");
    menu->addAction("Connection Editor", this, SLOT(actionConnectionManager()),QKeySequence(Qt::CTRL + Qt::Key_M));
    menu->addAction("Snapshot manager", this, SLOT(actionSnaptshotManager()),QKeySequence(Qt::CTRL + Qt::Key_S));
    menu->addAction("Group manager", this, SLOT(actionTreeGroupManager()),QKeySequence(Qt::CTRL + Qt::Key_G));
    menu->addAction("Log Browser", this, SLOT(actionLogBrowser()),QKeySequence(Qt::CTRL + Qt::Key_L));
    menu->addAction("Application Error Browser", this, SLOT(actionApplicationLogBrowser()),QKeySequence(Qt::CTRL + Qt::Key_E));

    //Data
    menu = main_menu_bar.addMenu("&Algorithm");
    menu->addAction("Script Manager", this, SLOT(actionScriptManager()),QKeySequence(Qt::CTRL + Qt::Key_P));

    //Data
    menu = main_menu_bar.addMenu("&Tools");
    menu->addAction("Node Monitor", this, SLOT(actionNewNodeMonitor()),QKeySequence(Qt::CTRL + Qt::Key_T));
    menu->addSeparator();
    menu->addAction("Switch Network Domain...", this, SLOT(actionSwitchNetworkConfiguration()));
    menu->addSeparator();
    menu->addAction("Agent Setting...", this, SLOT(actionAgentSetting()));
    menu->addSeparator();
    menu->addAction("Preferences...", this, SLOT(actionPreferences()));
    main_menu_bar.show();
}

void MainController::initConfigurationsMenu(QMenu *menu_configurations) {
    QAction *action_configuration = NULL;
    QActionGroup* group_configuration = new QActionGroup(this);
    QStringList networ_configurations = PreferenceManager::getInstance()->getNetworkConfigurationNames();
    const QString current_setting = PreferenceManager::getInstance()->getActiveNetworkConfigurationName();
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

void MainController::selectedNetworkDomain(const QString& selected_domain_network) {
    //init menu bar
    initApplicationMenuBar();

    actionSearchNode();
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

void MainController::actionConnectionManager() {
    openInWindow(new ConnectionEditor());
}

void MainController::actionApplicationLogBrowser() {
    if(application_error_widget == NULL) {
        openInWindow(application_error_widget = new ApplicationErrorLogging());
        connect(application_error_widget,
                SIGNAL(destroyed(QObject*)),
                SLOT(actionCloseWidget(QObject*)));
    } else {
        application_error_widget->show();
        application_error_widget->activateWindow();
        application_error_widget->raise();
    }
}

void MainController::actionCloseWidget(QObject *widget) {
    application_error_widget = NULL;
}

void MainController::actionSwitchNetworkConfiguration() {
    QWindowList opened_windows =  QGuiApplication::allWindows();
    for (uint i = 0 ; i< opened_windows.size() ; i++) {
        qDebug() << QString("Close window -> %1").arg(opened_windows.at(i)->title());
        opened_windows.at(i)->close();
    }

    //start network domain selector
    SelectNetworkDomain network_dmoain_selector;
    connect(&network_dmoain_selector,
            SIGNAL(networkDomainSelected(QString)),
            SLOT(selectedNetworkDomain(QString)));
    network_dmoain_selector.exec();
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
    pref_dialog.exec();
}

void MainController::actionAgentSetting() {
    openInWindow(new AgentSetting());
}
