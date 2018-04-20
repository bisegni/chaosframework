#ifndef DATASETATTRIBUTEREADER_H
#define DATASETATTRIBUTEREADER_H

#include "AttributeReader.h"

#include <QObject>
#include <QMap>
#include <QSharedPointer>

class DatasetAttributeReader :
        public AttributeReader {
    Q_OBJECT
public:
    explicit DatasetAttributeReader(QSharedPointer<chaos::common::data::CDataWrapper> _attribute_description,
                                    QObject *parent = 0);
    QString getName();
    QString getDescription();
    int32_t getType();
    std::vector<unsigned int> getSubtype();
    bool isMandatory();
    int getDirection();
signals:

public slots:

private:
    QSharedPointer<chaos::common::data::CDataWrapper> attribute_description;
};

#endif // DATASETATTRIBUTEREADER_H
