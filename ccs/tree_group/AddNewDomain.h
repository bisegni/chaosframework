#ifndef ADDNEWDOMAIN_H
#define ADDNEWDOMAIN_H

#include <QDialog>

namespace Ui {
class AddNewDomain;
}

class AddNewDomain : public QDialog
{
    Q_OBJECT

public:
    explicit AddNewDomain(QWidget *parent = 0);
    ~AddNewDomain();
    QString getDomainName();
    QString getRootName();
private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

private:
    Ui::AddNewDomain *ui;
};

#endif // ADDNEWDOMAIN_H
