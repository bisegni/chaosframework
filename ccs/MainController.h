#ifndef MAINCONTROLLER_H
#define MAINCONTROLLER_H

#include "MainWindow.h"

#include <QApplication>
#include <QSplashScreen>

#include <ChaosMetadataServiceClient/ChaosMetadataServiceClient.h>

class MainController
{
    MainWindow w;
    std::auto_ptr<QSplashScreen> splash;
public:
    MainController();
    ~MainController();
    void init(int argc, char **argv, QApplication &a);
    void deinit();
};

#endif // MAINCONTROLLER_H
