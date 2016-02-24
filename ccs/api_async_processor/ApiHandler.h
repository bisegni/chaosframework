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
                           QSharedPointer<chaos::common::data::CDataWrapper> api_result);


    //!Api has been give an error
    virtual void onApiError(const QString& tag,
                            QSharedPointer<chaos::CException> api_exception);

    //! api has gone in timeout
    virtual void onApiTimeout(const QString& tag);

    virtual void apiHasStarted(const QString& tag);

    virtual void apiHasEnded(const QString& tag);
};

#endif // APIHANDLER_H
