#ifndef APIHANDLER_H
#define APIHANDLER_H

#include <ChaosMetadataServiceClient/ChaosMetadataServiceClient.h>

#include <QDebug>
#include <QString>
#include <QSharedPointer>

class ApiHandler {
public:

    //!Api has ben called successfully
    virtual void onApiDone(const QString& tag,
                           QSharedPointer<chaos::common::data::CDataWrapper> api_result){
        qDebug()<< "ChaosWidgetCompanion-"<<tag<<"-onApiDone-"<< QString::fromStdString(api_result->getJSONString());
    }


    //!Api has been give an error
    virtual void onApiError(const QString& tag,
                            QSharedPointer<chaos::CException> api_exception) {
        qDebug()<< "ChaosWidgetCompanion-"<<tag<<"-onApiError-"<< api_exception->what();
    }

    //! api has gone in timeout
    virtual void onApiTimeout(const QString& tag) {
        qDebug()<< "ChaosWidgetCompanion-"<<tag<<"-onApiTimeout";
    }

    virtual void apiHasStarted(const QString& tag) {
        qDebug()<< "ChaosWidgetCompanion-"<<tag<<"-apiHasStarted";
    }

    virtual void apiHasEnded(const QString& tag) {
        qDebug()<< "ChaosWidgetCompanion-"<<tag<<"-apiHasEnded";
    }
};

#endif // APIHANDLER_H
