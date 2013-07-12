/*
 *	Main.cpp
 *	!CHOAS
 *	Created by Bisegni Claudio.
 *
 *    	Copyright 2012 INFN, National Institute of Nuclear Physics
 *
 *    	Licensed under the Apache License, Version 2.0 (the "License");
 *    	you may not use this file except in compliance with the License.
 *    	You may obtain a copy of the License at
 *
 *    	http://www.apache.org/licenses/LICENSE-2.0
 *
 *    	Unless required by applicable law or agreed to in writing, software
 *    	distributed under the License is distributed on an "AS IS" BASIS,
 *    	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    	See the License for the specific language governing permissions and
 *    	limitations under the License.
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
