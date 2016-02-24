#ifndef CHAOSWIDGETCOMPANION_H
#define CHAOSWIDGETCOMPANION_H

#include "../api_async_processor/ApiSubmitter.h"

class ChaosWidgetCompanion:
protected ApiHandler {
    QString p_node_uid;
    Q_PROPERTY(QString node_uid READ nodeUniqueID WRITE setNodeUniqueID)

public:
    ChaosWidgetCompanion();

    virtual void initChaosContent();
    virtual void deinitChaosContent();
    virtual void updateChaosContent();

    void setNodeUID(const QString& node_uid);
    const QString& nodeUID();
protected:
    //!submit api result for async wait termination
    void submitApiResult(const QString& api_tag,
                         chaos::metadata_service_client::api_proxy::ApiProxyResult api_result);

    //!Api has ben called successfully
    void onApiDone(const QString& tag,
                           QSharedPointer<chaos::common::data::CDataWrapper> api_result);

    //!Api has been give an error
    void onApiError(const QString& tag,
                            QSharedPointer<chaos::CException> api_exception);

    //! api has gone in timeout
    void onApiTimeout(const QString& tag);

    void apiHasStarted(const QString& api_tag);

    void apiHasEnded(const QString& api_tag);

private:
    ApiSubmitter api_submitter;
};

#endif // CHAOSWIDGETCOMPANION_H
