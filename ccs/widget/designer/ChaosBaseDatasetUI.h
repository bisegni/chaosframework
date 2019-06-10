#ifndef CHAOSUIBASE_H
#define CHAOSUIBASE_H

#include <QWidget>
#include <QVariant>
class ChaosBaseDatasetUI:
        public QWidget {
    Q_OBJECT
    Q_PROPERTY(QString deviceID READ deviceID WRITE setDeviceID)
    Q_PROPERTY(DatasetType datasetType READ datasetType WRITE setDatasetType)
    Q_PROPERTY(QString attributeName READ attributeName WRITE setAttributeName)
public:
    enum DatasetType { System, Input, Output};
    Q_ENUM(DatasetType)

    ChaosBaseDatasetUI(QWidget *parent = nullptr);

    void setDeviceID(const QString &new_device_id);
    QString deviceID() const;

    void setDatasetType(const DatasetType &new_dataset_type);
    DatasetType datasetType() const;


    QString attributeName() const;
    void setAttributeName(const QString& new_attribute_name);

signals:
     void valueChanged(QVariant nval);

public slots:
    virtual void updateData(QString attribute_name,
                            QVariant attribute_value);

private slots:
    virtual void updateValue(QVariant variant_value) = 0;

private:
    QVariant p_value;
    QString p_device_id;
    QString p_attribute_name;
    DatasetType p_dataset_type;
};

#endif // CHAOSUIBASE_H
