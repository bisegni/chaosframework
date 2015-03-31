#ifndef ASYNCWORKER_H
#define ASYNCWORKER_H

#include <QObject>
#include <QRunnable>
#include <QList>
#include <QVariant>

class AsyncWorker :
        public QObject,
        public QRunnable
{
    Q_OBJECT
public:
    explicit AsyncWorker(QObject *parent = 0);
    ~AsyncWorker();
signals:
    void workdDone(QList<QVariant> result);
public slots:

};

#endif // ASYNCWORKER_H
