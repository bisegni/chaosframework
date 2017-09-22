#include "ErrorManager.h"

#include <QMessageBox>
#include <QThreadPool>
#include <QSqlQuery>
#include <QVariant>
#include <QDebug>
#include <QSqlError>

void ErrorManagerEventSignal::fireEntryUpdate() {
    emit(errorEntryUpdated());
}

ErrorManager::ErrorManager() {
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("ccs_error");
    if (!db.open()) {
        QMessageBox::critical(0, "Cannot open database",
                              "Unable to establish a database connection.\n"
                              "Click Cancel to exit.", QMessageBox::Cancel);
    }

    QSqlQuery query(db);
    query.exec("create table error (id integer primary key AUTOINCREMENT, "
               "timestamp integer,error_code integer, error_message varchar(255), error_domain varchar(255))");
}

ErrorManager::~ErrorManager() {}

int ErrorManager::clearError() {
    int err = 0;
    if (db.isOpen() == false) return err;
    db_mutex.lock();
    QSqlQuery query(db);
    query.exec("DELETE FROM error");
    query.exec("VACUUM");
    err = query.lastError().number();
    db.commit();
    db_mutex.unlock();
    signal_proxy.fireEntryUpdate();
    return err;
}

int ErrorManager::submiteError(QSharedPointer<chaos::CException> new_error) {
    int err = 0;
    if (db.isOpen() == false) return err;
    db_mutex.lock();
    QSqlQuery query(db);
    query.prepare("INSERT INTO error (timestamp, error_code, error_message, error_domain) "
                  "VALUES (:timestamp, :error_code, :error_message, :error_domain)");
    query.bindValue(":timestamp", (qulonglong)QDateTime::currentMSecsSinceEpoch());
    query.bindValue(":error_code", QVariant(new_error->errorCode));
    query.bindValue(":error_message", QString::fromStdString(new_error->errorMessage));
    query.bindValue(":error_domain", QString::fromStdString(new_error->errorDomain));
    query.exec();
    err = query.lastError().number();
    db.commit();
    db_mutex.unlock();
    signal_proxy.fireEntryUpdate();
    return err;
}

int ErrorManager::submiteError(int32_t error_code, const QString& error_message, const QString& error_domain) {
    int err = 0;
    if (db.isOpen() == false) return err;
    db_mutex.lock();
    QSqlQuery query(db);
    query.prepare("INSERT INTO error (timestamp, error_code, error_message, error_domain) "
                  "VALUES (:timestamp, :error_code, :error_message, :error_domain)");
    query.bindValue(":timestamp", (qulonglong)QDateTime::currentMSecsSinceEpoch());
    query.bindValue(":error_code", QVariant(error_code));
    query.bindValue(":error_message", error_message);
    query.bindValue(":error_domain", error_domain);
    query.exec();
    err = query.lastError().number();
    db.commit();
    db_mutex.unlock();
    signal_proxy.fireEntryUpdate();
    return err;
}

int ErrorManager::getErrorCount(uint64_t& record_number) {
    int err = 0;
    if (db.isOpen() == false) {
        record_number = 0;
        return err;
    }
    db_mutex.lock();
    QSqlQuery query(db);
    query.exec("select count(*) from error order by id asc");
    if(err == 0 &&
            query.next()) {
        record_number = query.value(0).toULongLong();
    }
    db_mutex.unlock();
    return err;
}

int ErrorManager::getErrorPage(QVector< QSharedPointer<ErrorEntry> >& page_result,
                               uint64_t page_len,
                               uint64_t last_id) {
    int err = 0;
    db_mutex.lock();
    QSqlQuery query(db);
    query.prepare("select * from error where id > :id  order by id asc limit :max_row");
    query.bindValue(":id", (qulonglong)last_id);
    query.bindValue(":max_row", (qulonglong)page_len);
    query.exec();
    while(query.next()) {
        QSharedPointer<ErrorEntry> entry(new ErrorEntry);
        entry->seq = query.value(0).toULongLong();
        entry->date_time = QDateTime::fromMSecsSinceEpoch(query.value(1).toULongLong());
        entry->exception = QSharedPointer<chaos::CException>(new chaos::CException(query.value(2).toInt(),
                                                                                   query.value(3).toString().toStdString(),
                                                                                   query.value(4).toString().toStdString()));
        page_result.push_back(entry);
    }
    db_mutex.unlock();
    return err;
}

int ErrorManager::getErrorEntryByPosition(QSharedPointer<ErrorEntry>& page_result,
                                          uint64_t entry_position) {
    int err = 0;
    db_mutex.lock();
    QSqlQuery query(db);
    query.prepare("select * from error  order by id asc limit 1 offset :start_position");
    query.bindValue(":start_position", (qulonglong)entry_position);
    query.exec();
    if(query.next()) {
        page_result.reset(new ErrorEntry);
        page_result->seq = query.value(0).toULongLong();
        page_result->date_time = QDateTime::fromMSecsSinceEpoch(query.value(1).toULongLong());
        page_result->exception = QSharedPointer<chaos::CException>(new chaos::CException(query.value(2).toInt(),
                                                                                         query.value(3).toString().toStdString(),
                                                                                         query.value(4).toString().toStdString()));
    }
    db_mutex.unlock();
    return err;
}
