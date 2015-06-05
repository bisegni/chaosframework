#include "CommandDescription.h"

CommandDescription::CommandDescription(const QString& _alias, const QString& _description,  QObject *parent):
    QObject(parent),
    alias(_alias),
    description(_description){

}

