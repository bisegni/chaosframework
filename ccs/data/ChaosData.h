#ifndef CHAOSDATA_H
#define CHAOSDATA_H

#include <QObject>

class ChaosData : public QObject
{
    Q_OBJECT
public:
    explicit ChaosData(QObject *parent = 0);
    ~ChaosData();

signals:
    void updateProperty(const QString& property_name, const QString& property_new_value);
public slots:
};

#endif // CHAOSDATA_H
