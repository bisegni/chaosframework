#ifndef COMMANDDESCRIPTION_H
#define COMMANDDESCRIPTION_H

#include <QObject>

class CommandDescription :
        public QObject {

    Q_OBJECT
public:
    QString alias;
    QString description;
    explicit CommandDescription(const QString& _alias,
                                const QString& _description,
                                QObject *parent = 0);
};

#endif // COMMANDDESCRIPTION_H
