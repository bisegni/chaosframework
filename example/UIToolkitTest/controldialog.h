/*
 *	ControlDialog.h
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
#ifndef CONTROLDIALOG_H
#define CONTROLDIALOG_H

#include <QDialog>
#include <chaos/ui_toolkit/HighLevelApi/DeviceController.h>
#include <boost/shared_ptr.hpp>

namespace Ui {
class ControlDialog;
}

class ControlDialog : public QDialog
{
    Q_OBJECT
    chaos::ui::DeviceController *deviceController;
    chaos::CUSchemaDB::RangeValueInfo attributerange;
    std::string attributeName;
    std::string deviceID;
    QWidget *controlWidget;
public:
    explicit ControlDialog(QWidget *parent = 0);
    ~ControlDialog();
    
    void initDialog( chaos::ui::DeviceController *_deviceController, string& _attributeName);
private slots:
    void on_buttonCommit_clicked();

    void on_buttonClose_clicked();

    void on_horizontalSlider_sliderMoved(int position);

private:
    Ui::ControlDialog *ui;
};

#endif // CONTROLDIALOG_H
