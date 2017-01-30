#ifndef ERRORMANAGER_H
#define ERRORMANAGER_H

#include <chaos/common/utility/Singleton.h>
#include <chaos/common/exception/CException.h>

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
