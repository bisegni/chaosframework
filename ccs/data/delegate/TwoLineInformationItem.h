#ifndef TwoLineInformationItem_H
#define TwoLineInformationItem_H

#include <QObject>
#include <QSharedPointer>
#include <QVariant>

class TwoLineInformationItem :
        public QObject {

    Q_OBJECT
public:
    QString     title;
    QString     description;
    QVariant    data;

    explicit TwoLineInformationItem(const QString& _title,
                                    const QString& _description,
                                    QVariant _data,
                                    QObject *parent = 0);
    explicit TwoLineInformationItem(QObject *parent = 0);
};

#endif // TwoLineInformationItem_H
