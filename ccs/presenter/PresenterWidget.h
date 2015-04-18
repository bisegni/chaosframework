#ifndef PRESENTERWIDGET_H
#define PRESENTERWIDGET_H

#include "../api_async_processor/ApiAsyncProcessor.h"

#include <ChaosMetadataServiceClient/ChaosMetadataServiceClient.h>

#include <QWidget>
#include <QMdiSubWindow>

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

private slots:

    void asyncApiResult(QString tag,
                                QSharedPointer<chaos::common::data::CDataWrapper> api_result);

    void asyncApiError(QString tag,
                               QSharedPointer<chaos::CException> api_exception);

     void asyncApiTimeout(QString tag);

protected:
    ApiAsyncProcessor api_processor;
    void setTabTitle(const QString& title);

    void closeTab();

    void addWidgetToPresenter(PresenterWidget *p_w);

    void showInformation(const QString& title,
                         const QString& sub_title,
                         const QString& information);

    //!submit api result for async wait termination
    void submitApiResult(const QString& api_tag,
                         chaos::metadata_service_client::api_proxy::ApiProxyResult api_result);

    //!Api has ben called successfully
    virtual void onApiDone(QString tag,
                           QSharedPointer<chaos::common::data::CDataWrapper> api_result);

    //!Api has been give an error
    virtual void onApiError(QString tag,
                            QSharedPointer<chaos::CException> api_exception);

    //! api has gone in timeout
    virtual void onApiTimeout(QString tag);
signals:

    void onStartWaitApi(QString api_tag);
    void onEndWaitApi(QString api_tag);
};

#endif // PRESENTERWIDGET_H
