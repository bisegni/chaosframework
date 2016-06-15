#ifndef SCRIPTINSTANCEMANAGERWIDGET_H
#define SCRIPTINSTANCEMANAGERWIDGET_H

#include "../api_async_processor/ApiSubmitter.h"

#include <QWidget>

namespace Ui {
class ScriptInstanceManagerWidget;
}

class ScriptInstanceManagerWidget :
        public QWidget,
        protected ApiHandler {
    Q_OBJECT

public:
    explicit ScriptInstanceManagerWidget(QWidget *parent = 0);
    ~ScriptInstanceManagerWidget();
protected:
    void onApiDone(const QString& tag,
                   QSharedPointer<chaos::common::data::CDataWrapper> api_result);
private slots:
    void on_pushButtonSearchInstances_clicked();

    void on_pushButtonAddNew_clicked();

    void on_pushButtonremoveInstance_clicked();

private:
    ApiSubmitter api_submitter;
    Ui::ScriptInstanceManagerWidget *ui;
};

#endif // SCRIPTINSTANCEMANAGERWIDGET_H
