#ifndef MAINCONTROLLER_H
#define MAINCONTROLLER_H

#include "mainwindow.h"

#include <QObject>
#include <QApplication>
#include <QSplashScreen>

#include <ChaosMetadataServiceClient/ChaosMetadataServiceClient.h>

class MainController:
        public QObject
{
    Q_OBJECT
    MainWindow w;
    std::auto_ptr<QSplashScreen> splash;
public:
    MainController();
    ~MainController();
    void init(int argc, char **argv, QApplication &a);
    void deinit();
private slots:
    void reconfigure();
};

#endif // MAINCONTROLLER_H
