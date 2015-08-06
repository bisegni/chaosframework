#ifndef AttributeReader_H
#define AttributeReader_H

#include <QObject>

class AttributeReader :
        public QObject {
    Q_OBJECT
public:
    explicit AttributeReader(QObject *parent = 0);

    virtual QString getName() = 0;

    virtual QString getDescription() = 0;

    virtual int32_t getType() = 0;

    virtual bool isMandatory() = 0;

signals:

public slots:
};

#endif // AttributeReader_H
