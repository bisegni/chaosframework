#ifndef CDATASETATTRIBUTELABEL_H
#define CDATASETATTRIBUTELABEL_H

#include <QLabel>
namespace chaos {
namespace common {
namespace data {
class CDataVariant;
}
}
}


class CDatasetAttributeLabel :
        public QLabel {
    Q_OBJECT
    Q_PROPERTY(QString deviceID READ deviceID WRITE setDeviceID)
    Q_PROPERTY(DatasetType datasetType READ datasetType WRITE setDatasetType)
public:
    enum DatasetType { System, Input, Output};
    Q_ENUM(DatasetType)
    explicit CDatasetAttributeLabel(QWidget *parent = nullptr);
    ~CDatasetAttributeLabel();

    void setDeviceID(const QString &newDeviceID);
    QString deviceID() const;

    void setDatasetType(const DatasetType &newDatasetType);
    DatasetType datasetType() const;
signals:

public slots:
    void updateData(QSharedPointer<chaos::common::data::CDataVariant> variantValue);
private:
    QString myDeviceID;
    DatasetType myDatasetType;
    //slots hiding
    void clear();
    void setMovie(QMovie * movie);
    void setNum(int num);
    void setNum(double num);
    void setPicture(const QPicture & picture);
    void setPixmap(const QPixmap &pixmap);
};

#endif // CDATASETATTRIBUTELABEL_H
