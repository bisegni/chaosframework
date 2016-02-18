#ifndef APIHANDLER_H
#define APIHANDLER_H

#include <ChaosMetadataServiceClient/ChaosMetadataServiceClient.h>

#include <QString>
#include <QSharedPointer>

class ApiHandler {
public:

    //!Api has ben called successfully
    virtual void onApiDone(const QString& tag,
                   QSharedPointer<chaos::common::data::CDataWrapper> api_result) = 0;

    //!Api has been give an error
    virtual void onApiError(const QString& tag,
                    QSharedPointer<chaos::CException> api_exception) = 0;

    //! api has gone in timeout
    virtual void onApiTimeout(const QString& tag) = 0;

    virtual void apiHasStarted(const QString& tag) = 0;

    virtual void apiHasEnded(const QString& tag) = 0;
};

#endif // APIHANDLER_H
