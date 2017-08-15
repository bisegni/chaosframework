/*
 * Copyright 2012, 2017 INFN
 *
 * Licensed under the EUPL, Version 1.2 or – as soon they
 * will be approved by the European Commission - subsequent
 * versions of the EUPL (the "Licence");
 * You may not use this work except in compliance with the
 * Licence.
 * You may obtain a copy of the Licence at:
 *
 * https://joinup.ec.europa.eu/software/page/eupl
 *
 * Unless required by applicable law or agreed to in
 * writing, software distributed under the Licence is
 * distributed on an "AS IS" basis,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied.
 * See the Licence for the specific language governing
 * permissions and limitations under the Licence.
 */
#include  <QtGui>
#if ( QT_VERSION < QT_VERSION_CHECK(5, 0, 0) )
#include <QtGui/QApplication>
#else
#include <QtWidgets/QApplication>
#endif

#include "mainwindow.h"
#ifndef Q_MOC_RUN
#include <chaos/ui_toolkit/ChaosUIToolkit.h>
#endif


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    chaos::ui::ChaosUIToolkit::getInstance()->init(argc, argv);
    MainWindow w;
    w.show();
    int returnValue = a.exec();
    chaos::ui::ChaosUIToolkit::getInstance()->deinit();
    return returnValue;
}
