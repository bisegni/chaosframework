#ifndef DRIVERDESCRIPTIONINPUTDIALOG_H
#define DRIVERDESCRIPTIONINPUTDIALOG_H

#include <QDialog>
#include <QModelIndex>
namespace Ui {
class DriverDescriptionInputDialog;
}

class DriverDescriptionInputDialog :
        public QDialog {
    Q_OBJECT
    int current_row;
    const QString& driver_name;
    const QString& driver_version;
    const QString& driver_init_parameter;
public:
    DriverDescriptionInputDialog(QWidget *parent = 0);
    DriverDescriptionInputDialog(int current_row,
                                 const QString& _driver_name,
                                 const QString& _driver_version,
                                 const QString& _driver_init_parameter,
                                 QWidget *parent = 0);
    ~DriverDescriptionInputDialog();

private slots:
    void endWork();

signals:
    void newDriverDescription(const QString& driver_name,
                              const QString& driver_version,
                              const QString& driver_init_parameter);
    void updateDriverDescription(int current_row,
                                 const QString& driver_name,
                                 const QString& driver_version,
                                 const QString& driver_init_parameter);

private:
    Ui::DriverDescriptionInputDialog *ui;
};

#endif // DRIVERDESCRIPTIONINPUTDIALOG_H
