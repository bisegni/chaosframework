#ifndef TwoLineInformationItem_H
#define TwoLineInformationItem_H

#include <QObject>
#include <QSharedPointer>
#include <chaos/common/data/CDataWrapper.h>

class TwoLineInformationItem :
        public QObject {

    Q_OBJECT
public:
    QString title;
    QString description;
    QSharedPointer<chaos::common::data::CDataWrapper> raw_data;

    explicit TwoLineInformationItem(const QString& _title,
                                    const QString& _description,
                                    QSharedPointer<chaos::common::data::CDataWrapper> _raw_data,
                                    QObject *parent = 0);
    explicit TwoLineInformationItem(QObject *parent = 0);
};

#endif // TwoLineInformationItem_H
