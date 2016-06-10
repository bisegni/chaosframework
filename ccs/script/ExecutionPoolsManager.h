#ifndef EXECUTIONPOOLSMANAGER_H
#define EXECUTIONPOOLSMANAGER_H

#include <QWidget>

#include "../presenter/PresenterWidget.h"

namespace Ui {
class ExecutionPoolsManager;
}

class ExecutionPoolsManager:
        public PresenterWidget {
    Q_OBJECT

public:
    explicit ExecutionPoolsManager(QWidget *parent = 0);
    ~ExecutionPoolsManager();
protected:
    bool isClosing();
    void initUI();
    void onApiDone(const QString& tag,
                   QSharedPointer<chaos::common::data::CDataWrapper> api_result);
private:
    Ui::ExecutionPoolsManager *ui;
};

#endif // EXECUTIONPOOLSMANAGER_H
