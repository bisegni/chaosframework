#ifndef COMMANDPARAMETERREADER_H
#define COMMANDPARAMETERREADER_H

#include "AttributeReader.h"

#include <QObject>
#include <QMap>
#include <QSharedPointer>

class CommandParameterReader :
        public AttributeReader {
    Q_OBJECT
public:
    explicit CommandParameterReader(QSharedPointer<chaos::common::data::CDataWrapper> _command_parameter_description,
                                    QObject *parent = 0);
    QString getName();
    QString getDescription();
    int32_t getType();
    bool isMandatory();
signals:

public slots:

private:
    QSharedPointer<chaos::common::data::CDataWrapper> command_parameter_description;
};

#endif // COMMANDPARAMETERREADER_H
