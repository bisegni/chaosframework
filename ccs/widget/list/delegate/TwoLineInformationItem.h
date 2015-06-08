#ifndef TwoLineInformationItem_H
#define TwoLineInformationItem_H

#include <QObject>

class TwoLineInformationItem :
        public QObject {

    Q_OBJECT
public:
    QString title;
    QString description;
    explicit TwoLineInformationItem(const QString& _title,
                                    const QString& _description,
                                    QObject *parent = 0);
    explicit TwoLineInformationItem(QObject *parent = 0);
};

#endif // TwoLineInformationItem_H
