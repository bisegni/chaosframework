#ifndef APPLICATIONERRORLOGGING_H
#define APPLICATIONERRORLOGGING_H

#include "../data/ApplicationErrorLoggingTableModel.h"
#include "../presenter/PresenterWidget.h"

namespace Ui {
class ApplicationErrorLogging;
}

class ApplicationErrorLogging :
        public PresenterWidget  {
    Q_OBJECT

public:
    explicit ApplicationErrorLogging(QWidget *parent = 0);
    ~ApplicationErrorLogging();
protected:
    void initUI();
    bool isClosing();
private slots:
    void on_pushButtonClearLog_clicked();
private:
    ApplicationErrorLoggingTableModel application_error_model;
    Ui::ApplicationErrorLogging *ui;
};

#endif // APPLICATIONERRORLOGGING_H
