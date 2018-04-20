#ifndef PRESENTERWIDGET_H
#define PRESENTERWIDGET_H

#include "../monitor/monitor.h"
#include "../api_async_processor/ApiSubmitter.h"

#include <QMap>
#include <QWidget>
#include <QVariant>
#include <QMdiSubWindow>

#define CHK_STR_AND_GET(p, x, alt)\
    (p->hasKey(x)?p->getStringValue(x):alt)

#define CHK_INT32_AND_GET(p, x, alt)\
    (p->hasKey(x)?p->getInt32Value(x):alt)

#define CHK_UINT32_AND_GET(p, x, alt)\
    (p->hasKey(x)?p->getUInt32Value(x):alt)

class CommandPresenter;

class PresenterWidget:
        public QWidget,
        protected ApiHandler {
    Q_OBJECT
    friend class CommandPresenter;

    unsigned int submitted_api;
    ApiSubmitter api_submitter;
public:
    explicit PresenterWidget(QWidget *parent = NULL);
    ~PresenterWidget();

    void setThisTabIndex(int index);

    //! Called befor window is closed
    /*!
     * Called before window is closed, here the implementation
     * can dispose resource
     * \brief isClosing
     * \return
     */
    virtual bool isClosing() = 0;
    virtual void initUI() = 0;

signals:
    void presenterWidgetClosed();

public slots:
    void closeTab();

private:
    void showEvent( QShowEvent* event ) Q_DECL_OVERRIDE;
    void closeEvent(QCloseEvent *event) Q_DECL_OVERRIDE;
    void addDefaultNodeAction(QWidget *contextual_menu_parent);
private slots:
    void generalContextualMenuActionTrigger();
protected slots:
    virtual void startHealtMonitorAction();
    virtual void stopHealtMonitorAction();
    virtual void monitorHandlerUpdateAttributeValue(const QString& key,
                                                    const QString& attribute_name,
                                                    const QVariant& attribute_value);
protected:
    ApiAsyncProcessor api_processor;

    void setTitle(const QString& title);

    void launchPresenterWidget(PresenterWidget *p_w);

    void showInformation(const QString& title,
                         const QString& sub_title,
                         const QString& information);

    void addNodeToHealtMonitor(const QString& node);

    void removeNodeToHealtMonitor(const QString& node);

    void manageWidgetStateOnForValue(const QString& value);

    QString getDatasetKeyFromNodeKey(const QString& node_key,
                                     unsigned int dataset_type);
    QString getHealttKeyFromNodeKey(const QString& node_key);
    //local monitor handler management
    void registerMonitorHandler(const QString& monitor_key,
                                const unsigned int dataset_type,
                                unsigned int quantum_multiplier,
                                AbstractAttributeHandler *monitor_attribute_handler);
    void unregisterMonitorHandler(const QString& monitor_key,
                                  const unsigned int dataset_type,
                                  unsigned int quantum_multiplier,
                                  AbstractAttributeHandler *monitor_attribute_handler);
    void registerMonitorHandler(const std::string& monitor_key,
                                const unsigned int dataset_type,
                                unsigned int quantum_multiplier,
                                AbstractAttributeHandler *monitor_attribute_handler);
    void unregisterMonitorHandler(const std::string& monitor_key,
                                  const unsigned int dataset_type,
                                  unsigned int quantum_multiplier,
                                  AbstractAttributeHandler *monitor_attribute_handler);
    void registerHealtMonitorHandler(const QString& monitor_key,
                                     unsigned int quantum_multiplier,
                                     AbstractAttributeHandler *monitor_attribute_handler);
    void unregisterHealtMonitorHandler(const QString& monitor_key,
                                       unsigned int quantum_multiplier,
                                       AbstractAttributeHandler *monitor_attribute_handler);
    void registerHealtMonitorHandler(const std::string& monitor_key,
                                     unsigned int quantum_multiplier,
                                     AbstractAttributeHandler *monitor_attribute_handler);
    void unregisterHealtMonitorHandler(const std::string& monitor_key,
                                       unsigned int quantum_multiplier,
                                       AbstractAttributeHandler *monitor_attribute_handler);
    //contextual menu utility
    void registerWidgetForContextualMenu(QWidget *contextual_menu_parent,
                                         QMap<QString, QVariant> *widget_contextual_menu_action,
                                         bool add_default_node_action);
    void registerWidgetForContextualMenu(QWidget *contextual_menu_parent,
                                         QVector< QPair<QString, QVariant> >& widget_contextual_menu_action,
                                         bool add_default_node_action);
    virtual void contextualMenuActionTrigger(const QString& cm_title,
                                             const QVariant& cm_data);

    QAction *returnWidgetAction(QWidget *parent,
                                const QString& action_name);

    void contextualMenuActionSetEnable(QWidget *parent,
                                       const QString& action_name,
                                       bool enabled);
    void contextualMenuActionSetVisible(QWidget *parent,
                                        const QString& action_name,
                                        bool visible);
    void contextualMenuActionSetData(QWidget *parent,
                                     const QString& action_name,
                                     const QVariant& action_data);

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
};

#endif // PRESENTERWIDGET_H
