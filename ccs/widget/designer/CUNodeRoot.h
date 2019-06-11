#ifndef CUNODEROOT_H
#define CUNODEROOT_H

#include <QObject>
#include <QVariant>

/**
 * @brief The CUNodeRoot class
 *
 * This class bastract the control unit in the synoptic layout. For every control unit decalred
 * in every widget is create an unique CUNodeRoot taht manage the principal slot and singl for that cu.
 * This node will forward the update of state and dataset values and abstract the way command and
 * setting can be sent to the cu. All chaos widget ui will be connected for read an write data from
 * CU to their respective node root.
 */
class CUNodeRoot : public QObject {
    Q_OBJECT
public:
    explicit CUNodeRoot(QString device_id,
                        QObject *parent = nullptr);

    void setCurrentAttributeValue(QString attribute_name,
                                  QVariant attribute_value);
signals:
    /**
     * @brief updateDatasetAttribute send signel when a new update on a dataset attribute is triggered
     * @param attribute_name
     * @param attribute_value
     */
    void updateDatasetAttribute(QString attribute_name,
                                QVariant attribute_value);
public slots:

private:
    const QString m_device_id;
};

#endif // CUNODEROOT_H
