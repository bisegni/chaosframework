#include "MainController.h"

#include <QStyleFactory>
#include <QThreadPool>
#include <QTextStream>
#include <QDebug>
#include <QFile>
using namespace chaos::metadata_service_client;

Q_DECLARE_METATYPE(QSharedPointer<chaos::CException>);
Q_DECLARE_METATYPE(QSharedPointer<chaos::common::data::CDataWrapper>);

MainController::MainController()
{

}

MainController::~MainController()
{

}

void MainController::init(int argc, char **argv, QApplication& a) {
    qRegisterMetaType< QSharedPointer<chaos::CException> >();
    qRegisterMetaType< QSharedPointer<chaos::common::data::CDataWrapper> >();
    //set the dark fusion style
    a.setStyle(QStyleFactory::create("Fusion"));
    /* QFile f(":dark_orange/style.qss");
    if (!f.exists())
    {
        printf("Unable to set stylesheet, file not found\n");
    }
    else
    {
        f.open(QFile::ReadOnly | QFile::Text);
        QTextStream ts(&f);
        a.setStyleSheet(ts.readAll());
    }*/
    //    QPalette palette;
    //    palette.setColor(QPalette::Window, QColor(53,53,53));
    //    palette.setColor(QPalette::WindowText, Qt::white);
    //    palette.setColor(QPalette::Base, QColor(15,15,15));
    //    palette.setColor(QPalette::AlternateBase, QColor(53,53,53));
    //    palette.setColor(QPalette::ToolTipBase, Qt::white);
    //    palette.setColor(QPalette::ToolTipText, Qt::white);
    //    palette.setColor(QPalette::Text, Qt::white);
    //    palette.setColor(QPalette::Button, QColor(53,53,53));
    //    palette.setColor(QPalette::ButtonText, Qt::white);
    //    palette.setColor(QPalette::BrightText, Qt::red);

    //    palette.setColor(QPalette::Highlight, QColor(142,45,197).lighter());
    //    palette.setColor(QPalette::HighlightedText, Qt::black);
    //    a.setPalette(palette);

    //    a.setStyleSheet("QToolTip { color: #ffffff; background-color: #2a82da; border: 1px solid white; }");
    QApplication::setApplicationName("!CHAOS Control Studio");
    QApplication::setApplicationVersion("0.0.1-alpha");
    QApplication::setOrganizationName("INFN-LNF");

    a.setStyle(QStyleFactory::create("Fusion"));

    QPalette darkPalette;
    darkPalette.setColor(QPalette::Window, QColor(53,53,53));
    darkPalette.setColor(QPalette::WindowText, Qt::white);
    darkPalette.setColor(QPalette::Base, QColor(25,25,25));
    darkPalette.setColor(QPalette::AlternateBase, QColor(53,53,53));
    darkPalette.setColor(QPalette::ToolTipBase, Qt::white);
    darkPalette.setColor(QPalette::ToolTipText, Qt::white);
    darkPalette.setColor(QPalette::Text, Qt::white);
    darkPalette.setColor(QPalette::Button, QColor(53,53,53));
    darkPalette.setColor(QPalette::ButtonText, Qt::white);
    darkPalette.setColor(QPalette::BrightText, Qt::red);
    darkPalette.setColor(QPalette::Link, QColor(42, 130, 218));

    darkPalette.setColor(QPalette::Highlight, QColor(42, 130, 218));
    darkPalette.setColor(QPalette::HighlightedText, Qt::black);

    a.setPalette(darkPalette);
    a.setStyleSheet("QToolTip { color: #ffffff; background-color: #2a82da; border: 1px solid white; }");

    QApplication::setOrganizationDomain("http://chaos.infn.it");

    //initialize !CHAOS metadata service client
    ChaosMetadataServiceClient::getInstance()->init(argc, argv);

    //set thread pool thread size
    qDebug() << "Thread pool of size:" << QThreadPool::globalInstance()->maxThreadCount();

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
