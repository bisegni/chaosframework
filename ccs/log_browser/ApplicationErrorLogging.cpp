#include "ApplicationErrorLogging.h"
#include "ui_ApplicationErrorLogging.h"
#include "../error/ErrorManager.h"

ApplicationErrorLogging::ApplicationErrorLogging(QWidget *parent) :
    PresenterWidget(parent),
    ui(new Ui::ApplicationErrorLogging) {
    ui->setupUi(this);
}

ApplicationErrorLogging::~ApplicationErrorLogging() {
    delete ui;
}

void ApplicationErrorLogging::on_pushButtonClearLog_clicked() {
    ErrorManager::getInstance()->clearError();
}

void ApplicationErrorLogging::initUI() {
    ui->tableViewApplicationLogging->setModel(&application_error_model);
}
bool ApplicationErrorLogging::isClosing() {
    return true;
}
