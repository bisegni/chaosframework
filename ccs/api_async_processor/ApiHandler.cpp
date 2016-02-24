#include "ApiHandler.h"


//!Api has ben called successfully
void ApiHandler::onApiDone(const QString& tag,
                           QSharedPointer<chaos::common::data::CDataWrapper> api_result){
    qDebug()<< "ChaosWidgetCompanion-"<<tag<<"-onApiDone-"<< QString::fromStdString(api_result->getJSONString());
}


//!Api has been give an error
void ApiHandler::onApiError(const QString& tag,
                            QSharedPointer<chaos::CException> api_exception) {
    qDebug()<< "ChaosWidgetCompanion-"<<tag<<"-onApiError-"<< api_exception->what();
}

//! api has gone in timeout
void ApiHandler::onApiTimeout(const QString& tag) {
    qDebug()<< "ChaosWidgetCompanion-"<<tag<<"-onApiTimeout";
}

void ApiHandler::apiHasStarted(const QString& tag) {
    qDebug()<< "ChaosWidgetCompanion-"<<tag<<"-apiHasStarted";
}

void ApiHandler::apiHasEnded(const QString& tag) {
    qDebug()<< "ChaosWidgetCompanion-"<<tag<<"-apiHasEnded";
}
