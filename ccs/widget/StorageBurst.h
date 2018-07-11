#ifndef STORAGEBURST_H
#define STORAGEBURST_H

#include <QWidget>
#include "ChaosWidgetCompanion.h"
namespace Ui {
class StorageBurst;
}

class StorageBurst :
        public QWidget,
        public ChaosWidgetCompanion {
    Q_OBJECT

public:
    explicit StorageBurst(QWidget *parent = 0);
    ~StorageBurst();

private slots:
    void on_comboBox_currentIndexChanged(const QString &selected_item);

    void on_pushButtonSubmit_clicked();

private:
    void apiHasStarted(const QString& api_tag);
    void apiHasEnded(const QString& api_tag);
    void apiHasEndedWithError(const QString& api_tag,
                              QSharedPointer<chaos::CException> api_exception);
    Ui::StorageBurst *ui;
};

#endif // STORAGEBURST_H
