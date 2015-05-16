#ifndef PRESENTERWIDGET_H
#define PRESENTERWIDGET_H

#include "../api_async_processor/ApiAsyncProcessor.h"

#include <ChaosMetadataServiceClient/ChaosMetadataServiceClient.h>

#include <QMap>
#include <QWidget>
#include <QVariant>
#include <QMdiSubWindow>

#define GET_CHAOS_API_PTR(api_name)\
    chaos::metadata_service_client::ChaosMetadataServiceClient::getInstance()->getApiProxy<api_name>()

#define CHK_STR_AND_GET(p, x, alt)\
    (p->hasKey(x)?p->getStringValue(x):alt)

#define CHK_INT32_AND_GET(p, x, alt)\
    (p->hasKey(x)?p->getInt32Value(x):alt)

#define CHK_UINT32_AND_GET(p, x, alt)\
    (p->hasKey(x)?p->getUInt32Value(x):alt)

class CommandPresenter;

class PresenterWidget:
        public QWidget
{
    Q_OBJECT
    friend class CommandPresenter;

    QMdiSubWindow *editor_subwindow;
    CommandPresenter *presenter_instance;

public:
    explicit PresenterWidget(QWidget *parent = NULL);
    ~PresenterWidget();

    void setThisTabIndex(int index);

    virtual bool canClose() = 0;
    virtual void initUI() = 0;

private:
    void closeEvent(QCloseEvent *event) Q_DECL_OVERRIDE;
    void setSubWindow(QMdiSubWindow *_editor_subwindow);
    void addDefaultNodeAction(QWidget *contextual_menu_parent);
private slots:

    void asyncApiResult(const QString& tag,
                        QSharedPointer<chaos::common::data::CDataWrapper> api_result);

    void asyncApiError(const QString& tag,
                       QSharedPointer<chaos::CException> api_exception);

    void asyncApiTimeout(const QString& tag);

    void generalContextualMenuActionTrigger();
protected slots:
    virtual void startHealtMonitorAction();
    virtual void stopHealtMonitorAction();

protected:
    ApiAsyncProcessor api_processor;
    void setTabTitle(const QString& title);

    void closeTab();

    void addWidgetToPresenter(PresenterWidget *p_w);

    void showInformation(const QString& title,
                         const QString& sub_title,
                         const QString& information);

    void addNodeToHealtMonitor(const QString& node);

    void removeNodeToHealtMonitor(const QString& node);

    //contextual menu utility
    void registerWidgetForContextualMenu(QWidget *contextual_menu_parent,
                                         QMap<QString, QVariant> *widget_contextual_menu_action,
                                         bool add_default_node_action);

    virtual void contextualMenuActionTrigger(const QString& cm_title,
                                             const QVariant& cm_data);

    //!submit api result for async wait termination
    void submitApiResult(const QString& api_tag,
                         chaos::metadata_service_client::api_proxy::ApiProxyResult api_result);

    //!Api has ben called successfully
    virtual void onApiDone(const QString& tag,
                           QSharedPointer<chaos::common::data::CDataWrapper> api_result);

    //!Api has been give an error
    virtual void onApiError(const QString& tag,
                            QSharedPointer<chaos::CException> api_exception);

    //! api has gone in timeout
    virtual void onApiTimeout(const QString& tag);
signals:

    void onStartWaitApi(const QString& api_tag);
    void onEndWaitApi(const QString& api_tag);
};

#endif // PRESENTERWIDGET_H
