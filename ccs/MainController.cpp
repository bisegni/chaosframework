#include "MainController.h"

#include "node/data/CommandDescription.h"

#include <QStyleFactory>
#include <QThreadPool>
#include <QTimer>
#include <QTextStream>
#include <QDebug>
#include <QFile>
#include <QSettings>

#include <string>
using namespace chaos::metadata_service_client;

//declare metatype used in chaos
Q_DECLARE_METATYPE(QSharedPointer<chaos::CException>);
Q_DECLARE_METATYPE(QSharedPointer<chaos::common::data::CDataWrapper>);
Q_DECLARE_METATYPE(QSharedPointer<CommandDescription>);
Q_DECLARE_METATYPE (std::string)
Q_DECLARE_METATYPE(int32_t)
Q_DECLARE_METATYPE(int64_t)
Q_DECLARE_METATYPE(boost::shared_ptr<chaos::common::data::SerializationBuffer>)
Q_DECLARE_METATYPE(boost::shared_ptr<chaos::common::data::CDataWrapper>)
Q_DECLARE_METATYPE(boost::shared_ptr<chaos::CException>)

MainController::MainController() {
}

MainController::~MainController()
{

}

void MainController::init(int argc, char **argv, QApplication& a) {
    //register chaos metatype
    qRegisterMetaType< QSharedPointer<chaos::CException> >();
    qRegisterMetaType< QSharedPointer<chaos::common::data::CDataWrapper> >();
    qRegisterMetaType< QSharedPointer<CommandDescription> >();
    qRegisterMetaType<std::string>("std::string");
    qRegisterMetaType<int32_t>("int32_t");
    qRegisterMetaType<int64_t>("int64_t");
    qRegisterMetaType<boost::shared_ptr<chaos::common::data::SerializationBuffer> >();
    qRegisterMetaType<boost::shared_ptr<chaos::common::data::SerializationBuffer> >("boost::shared_ptr<chaos::common::data::SerializationBuffer>&");
    qRegisterMetaType<boost::shared_ptr<chaos::common::data::CDataWrapper> >("boost::shared_ptr<chaos::common::data::CDataWrapper>");
    qRegisterMetaType<boost::shared_ptr<chaos::CException> >("chaos::CException");

    //set the dark fusion style
    a.setStyle(QStyleFactory::create("Fusion"));

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
    a.setStyleSheet("QToolTip { color: #ffffff; background-color: #2a82da; border: 1px solid white; }"
                    "QHeaderView {font-size: 11pt;}"
                    "QComboBox {font-size: 11pt;}");

    //show main window
    splash.reset(new QSplashScreen(pixmap));

    splash->showMessage(QObject::tr("Starting !CHAOS layer..."),
                        Qt::AlignLeft | Qt::AlignBottom, Qt::lightGray);
    a.processEvents();
    //initialize !CHAOS metadata service client
    ChaosMetadataServiceClient::getInstance()->init(argc, argv);

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
    //clear the qthread pool
    QThreadPool::globalInstance()->clear();
    QThreadPool::globalInstance()->waitForDone();

    //! deinitialize !CHOAS  metadata service client
    ChaosMetadataServiceClient::getInstance()->deinit();

    qDebug() << "!CHAOS Control Studio closed!";
}
