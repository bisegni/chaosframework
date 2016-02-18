#include "TwoLineInformationItem.h"

TwoLineInformationItem::TwoLineInformationItem(const QString& _title,
                                               const QString& _description, QSharedPointer<chaos::common::data::CDataWrapper> _raw_data,
                                               QObject *parent):
    QObject(parent),
    title(_title),
    description(_description),
    raw_data(_raw_data){}

TwoLineInformationItem::TwoLineInformationItem(QObject *parent):
    QObject(parent),
    title("title"),
    description("description"){}

