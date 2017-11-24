#ifndef ERRORMANAGER_H
#define ERRORMANAGER_H

#include <QObject>
#include <QMutex>
#include <QVector>
#include <QSqlDatabase>
#include <QSharedPointer>
#include <QDateTime>

struct ErrorEntry {
    uint64_t seq;
    QDateTime date_time;
    QSharedPointer<chaos::CException> exception;
};

class ErrorManager;

class ErrorManagerEventSignal:
        public QObject {
    friend class ErrorManager;
    Q_OBJECT
protected:
    void fireEntryUpdate();
signals:
    void errorEntryUpdated();
};

class ErrorManager:
        public chaos::common::utility::Singleton<ErrorManager> {
    friend class chaos::common::utility::Singleton<ErrorManager>;
public:
    ErrorManagerEventSignal signal_proxy;
    int submiteError(QSharedPointer<chaos::CException> new_error);
    int submiteError(int32_t error_code,
                     const QString& error_message,
                     const QString& error_domain);
    int getErrorCount(uint64_t &record_number);
    int getErrorPage(QVector< QSharedPointer<ErrorEntry> >& page_result,
                     uint64_t page_len,
                     uint64_t last_id = 0);
    int getErrorEntryByPosition(QSharedPointer<ErrorEntry>& page_result,
                                uint64_t entry_position);
    int clearError();
signals:

public slots:
private:
    QMutex db_mutex;
    QSqlDatabase  db;
    ErrorManager();
    ~ErrorManager();
};

#endif // ERRORMANAGER_H
