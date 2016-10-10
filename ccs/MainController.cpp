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
#include <QSettings>

#include <string>

#include <ChaosMetadataServiceClient/ChaosMetadataServiceClient.h>
#include "widget/CStateVisiblePushButton.h"

using namespace chaos::metadata_service_client;

//declare metatype used in chaos
Q_DECLARE_METATYPE(std::string)
Q_DECLARE_METATYPE(int32_t)
Q_DECLARE_METATYPE(int64_t)
Q_DECLARE_METATYPE(uint64_t)
Q_DECLARE_METATYPE(QSharedPointer<ChaosByteArray>)
Q_DECLARE_METATYPE(QSharedPointer<CommandReader>)
Q_DECLARE_METATYPE(QSharedPointer<CommandParameterReader>)
Q_DECLARE_METATYPE(QSharedPointer<DatasetReader>)
Q_DECLARE_METATYPE(QSharedPointer<DatasetAttributeReader>)
Q_DECLARE_METATYPE(QSharedPointer<chaos::CException>)
Q_DECLARE_METATYPE(QSharedPointer<chaos::common::data::CDataWrapper>)
Q_DECLARE_METATYPE(QSharedPointer<TwoLineInformationItem>)
Q_DECLARE_METATYPE(QSharedPointer<chaos::metadata_service_client::api_proxy::node::CommandTemplate>)
Q_DECLARE_METATYPE(boost::shared_ptr<chaos::metadata_service_client::api_proxy::node::CommandTemplate>)
Q_DECLARE_METATYPE(boost::shared_ptr<chaos::common::data::SerializationBuffer>)
Q_DECLARE_METATYPE(boost::shared_ptr<chaos::common::data::CDataWrapper>)
Q_DECLARE_METATYPE(boost::shared_ptr<chaos::CException>)
Q_DECLARE_METATYPE(chaos::metadata_service_client::node_monitor::OnlineState)
Q_DECLARE_METATYPE(chaos::service_common::data::node::NodeInstance)

MainController::MainController() {
}

MainController::~MainController()
{

}

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

    QPixmap pixmap(":splash/main_splash.png");
    QApplication::setApplicationName("chaos_control_studio");
    QApplication::setApplicationVersion("0.0.1-alpha");
    QApplication::setOrganizationName("INFN-LNF");
    QApplication::setOrganizationDomain("chaos.infn.it");

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
    a.setStyleSheet("QWidget {font-size: 12pt;}"
                    "QToolTip { color: #ffffff; background-color: #2a82da; border: 1px solid white; }");
#else

#endif


    //show main window
    splash.reset(new QSplashScreen(pixmap));

    splash->showMessage(QObject::tr("Starting !CHAOS layer..."),
                        Qt::AlignLeft | Qt::AlignBottom, Qt::lightGray);
    a.processEvents();
    //initialize !CHAOS metadata service client
    ChaosMetadataServiceClient::getInstance()->init(argc, argv);
    ChaosMetadataServiceClient::getInstance()->start();
    splash->showMessage(QObject::tr("!CHAOS Control Studio Initilized!"),
                        Qt::AlignLeft | Qt::AlignBottom, Qt::lightGray);
    a.processEvents();
    //set thread pool thread size
    qDebug() << "Thread pool of size:" << QThreadPool::globalInstance()->maxThreadCount();

    splash->show();

    //configure from preference value
    w.reconfigure();

    //show main window
    w.show();
}

void MainController::deinit() {
    //close all editors
    w.disposeResource();
    //clear the qthread pool
    QThreadPool::globalInstance()->clear();
    QThreadPool::globalInstance()->waitForDone();

    //! deinitialize !CHOAS  metadata service client
    ChaosMetadataServiceClient::getInstance()->stop();
    ChaosMetadataServiceClient::getInstance()->deinit();

    qDebug() << "!CHAOS Control Studio closed!";
}
