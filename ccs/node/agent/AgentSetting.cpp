#include "AgentSetting.h"
#include "ui_AgentSetting.h"

#include <QIntValidator>

using namespace chaos::service_common::data::agent;
using namespace chaos::metadata_service_client::api_proxy;

AgentSetting::AgentSetting(QWidget *parent) :
    PresenterWidget(parent),
    ui(new Ui::AgentSetting) {
    ui->setupUi(this);
}

AgentSetting::~AgentSetting() {
    delete ui;
}


void AgentSetting::initUI() {
    QIntValidator *v = new QIntValidator(this);
    v->setBottom(0);
    ui->lineEditExpirationTime->setValidator(v);
    submitApiResult("AgentSetting::loadSetting",
                    GET_CHAOS_API_PTR(agent::GetManagementConfiguration)->execute());
}

bool AgentSetting::isClosing() {
    return true;
}

void AgentSetting::onApiDone(const QString& tag,
                             QSharedPointer<chaos::common::data::CDataWrapper> api_result) {
    if(tag.compare("AgentSetting::loadSetting") == 0) {
        agent_setting = agent::GetManagementConfiguration::deserialize(*api_result);
        //fill ui
        QMetaObject::invokeMethod(this,
                                  "updateUI",
                                  Qt::QueuedConnection);
    }
}

void AgentSetting::on_pushButtonSaveSetting_clicked() {
    agent_setting.log_expiration_in_seconds = ui->lineEditExpirationTime->text().toUInt();
    submitApiResult("AgentSetting::set",
                    GET_CHAOS_API_PTR(agent::SetManagementConfiguration)->execute(agent_setting));
}

void AgentSetting::on_pushButtonUpdateSetting_clicked() {
    submitApiResult("AgentSetting::loadSetting",
                    GET_CHAOS_API_PTR(agent::GetManagementConfiguration)->execute());
}

void AgentSetting::updateUI() {
    ui->lineEditExpirationTime->setText(QString::number(agent_setting.log_expiration_in_seconds));
}
