#include "TwoLineInformationItem.h"

TwoLineInformationItem::TwoLineInformationItem(const QString& _title,
                                               const QString& _description,
                                               QVariant _data,
                                               QObject *parent):
    QObject(parent),
    title(_title),
    description(_description),
    data(_data){}

TwoLineInformationItem::TwoLineInformationItem(QObject *parent):
    QObject(parent),
    title("title"),
    description("description"){}

