#ifndef COMMANDPARAMETERREADER_H
#define COMMANDPARAMETERREADER_H

#include <QObject>
#include <QMap>
#include <QSharedPointer>

#include <chaos/common/chaos_constants.h>
#include <chaos/common/data/CDataWrapper.h>

class CommandParameterReader :
        public QObject {
    Q_OBJECT
public:
    explicit CommandParameterReader(QSharedPointer<chaos::common::data::CDataWrapper> _command_parameter_description,
                                    QObject *parent = 0);
    QString getParameterName();
    QString getParameterDescription();
    int32_t getParameterType();
    bool isMandatory();
signals:

public slots:

private:
    QSharedPointer<chaos::common::data::CDataWrapper> command_parameter_description;
};

#endif // COMMANDPARAMETERREADER_H
