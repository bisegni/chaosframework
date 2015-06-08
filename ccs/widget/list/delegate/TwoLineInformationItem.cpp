#include "TwoLineInformationItem.h"

TwoLineInformationItem::TwoLineInformationItem(const QString& _title,
                                               const QString& _description,
                                               QObject *parent):
    QObject(parent),
    title(_title),
    description(_description){}

TwoLineInformationItem::TwoLineInformationItem(QObject *parent):
    QObject(parent),
    title("title"),
    description("description"){}

