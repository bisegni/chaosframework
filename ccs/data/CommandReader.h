#ifndef CHAOSDATA_H
#define CHAOSDATA_H

#include "CommandParameterReader.h"

#include <QMap>
#include <QObject>
#include <QSharedPointer>

#include <chaos/common/chaos_constants.h>
#include <chaos/common/data/CDataWrapper.h>

class CommandReader :
        public QObject {
    Q_OBJECT
public:
    explicit CommandReader(QSharedPointer<chaos::common::data::CDataWrapper> command_description,
                           QObject *parent = 0);
    ~CommandReader();
    QSharedPointer<CommandParameterReader> getCommandParameter(const QString& parameter_name);
    QList<QString> getParameterList();
    QList< QSharedPointer<CommandParameterReader> > getCommandParameterList();
public slots:

private:
    QMap<QString, QSharedPointer<chaos::common::data::CDataWrapper> > command_parameter_cache;
    QSharedPointer<chaos::common::data::CDataWrapper> command_description;
};

#endif // CHAOSDATA_H
