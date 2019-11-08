#ifndef CUNODEROOT_H
#define CUNODEROOT_H

#include <QMap>
#include <QObject>
#include <QVariant>

#include "../../api_async_processor/ApiSubmitter.h"

#include "ChaosBaseDatasetUI.h"

/**
 * @brief The CUNodeRoot class
 *
 * This class bastract the control unit in the synoptic layout. For every control unit decalred
 * in every widget is create an unique CUNodeRoot taht manage the principal slot and singl for that cu.
 * This node will forward the update of state and dataset values and abstract the way command and
 * setting can be sent to the cu. All chaos widget ui will be connected for read an write data from
 * CU to their respective node root.
 */
class CUNodeRoot :
        public QObject,
        protected ApiHandler {
    Q_OBJECT
public:
    explicit CUNodeRoot(QString device_id,
                        QObject *parent = nullptr);

    void setCurrentAttributeValue(int dataset_type,
                                  QString attribute_name,
                                  QVariant attribute_value);
    void setOnlineState(int state);
signals:
    /**
     * @brief updateDatasetAttribute send signel when a new update on a dataset attribute is triggered
     * @param attribute_name
     * @param attribute_value
     */
    void updateDatasetAttribute(int dataset_type,
                                QString attribute_name,
                                QVariant attribute_value);

    void updateOnlineState(int state);

    //singla that infomr that change set has been applyed
    void changeSetCommitted();
public slots:
    void init();
    void start();
    void stop();
    void deinit();
    //add a new changeset attribute value
    void attributeChangeSetUpdated(QString,
                                   QString attribute_name,
                                   QVariant attribute_value);
    //!remove a change set
    void attributeChangeSetClear(QString device_id,
                                   QString attribute_name);
    //!apply all changeset
    void commitChangeSet();
private:
    ApiSubmitter api_submitter;
    const QString m_device_id;
    QMap<QString, QVariant> map_attribute_value;

    // api submittion handler
    void onApiDone(const QString& tag,
                   QSharedPointer<chaos::common::data::CDataWrapper> api_result);
    void onApiError(const QString& tag,
                    QSharedPointer<chaos::CException> api_exception);
    void onApiTimeout(const QString& tag) ;
    void apiHasStarted(const QString& api_tag);
    void apiHasEnded(const QString& api_tag);
    void apiHasEndedWithError(const QString& tag,
                              const QSharedPointer<chaos::CException> api_exception);
};

#endif // CUNODEROOT_H
