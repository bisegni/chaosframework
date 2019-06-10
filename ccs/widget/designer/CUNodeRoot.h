#ifndef CUNODEROOT_H
#define CUNODEROOT_H

#include <QObject>

/**
 * @brief The CUNodeRoot class
 *
 * This class bastract the control unit in the synoptic layout. For every control unit decalred
 * in every widget is create an unique CUNodeRoot taht manage the principal slot and singl for that cu.
 * This node will forward the update of state and dataset values and abstract the way command and
 * setting can be sent to the cu. All chaos widget ui will be connected for read an write data from
 * CU to their respective node root.
 */
class CUNodeRoot : public QObject
{
    Q_OBJECT
public:
    explicit CUNodeRoot(QObject *parent = nullptr);

signals:

public slots:
};

#endif // CUNODEROOT_H
