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
    boost::shared_ptr<chaos::ui::DeviceController> deviceController;
    chaos::CUSchemaDB::RangeValueInfo attributerange;
    std::string attributeName;
    std::string deviceID;
    QWidget *controlWidget;
public:
    explicit ControlDialog(QWidget *parent = 0);
    ~ControlDialog();
    
    void initDialog( boost::shared_ptr<chaos::ui::DeviceController>& _deviceController, string& _attributeName);
private slots:
    void on_buttonCommit_clicked();

    void on_buttonClose_clicked();

private:
    Ui::ControlDialog *ui;
};

#endif // CONTROLDIALOG_H
