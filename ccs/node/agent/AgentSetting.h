#ifndef AGENTSETTING_H
#define AGENTSETTING_H

#include "../../presenter/PresenterWidget.h"

#include <QWidget>

namespace Ui {
class AgentSetting;
}

class AgentSetting:
        public PresenterWidget {
    Q_OBJECT
protected:
    void initUI();
    bool isClosing();
    void onApiDone(const QString& tag,
                   QSharedPointer<chaos::common::data::CDataWrapper> api_result);
public:
    explicit AgentSetting(QWidget *parent = 0);
    ~AgentSetting();

private slots:
    void updateUI();
    void on_pushButtonSaveSetting_clicked();
    void on_pushButtonUpdateSetting_clicked();

private:
    Ui::AgentSetting *ui;
    chaos::service_common::data::agent::AgentManagementSetting agent_setting;
};

#endif // AGENTSETTING_H
