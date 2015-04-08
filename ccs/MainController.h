#ifndef MAINCONTROLLER_H
#define MAINCONTROLLER_H

#include "mainwindow.h"

#include <QApplication>

#include <ChaosMetadataServiceClient/ChaosMetadataServiceClient.h>

class MainController
{
         MainWindow w;
public:
    MainController();
    ~MainController();
    void init(int argc, char **argv, QApplication &a);
    void deinit();
};

#endif // MAINCONTROLLER_H
