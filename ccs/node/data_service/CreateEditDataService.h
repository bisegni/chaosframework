#ifndef CREATEEDITDATASERVICE_H
#define CREATEEDITDATASERVICE_H

#include <QDialog>

namespace Ui {
class CreateEditDataService;
}

class CreateEditDataService : public QDialog
{
    Q_OBJECT
    int current_row;
public:
    explicit CreateEditDataService();
    explicit CreateEditDataService(int _current_row,
                                   const QString &unique_id,
                                   const QString &direct_io_address,
                                   int direct_io_endpoint);
    ~CreateEditDataService();
private slots:
    void endWork();

signals:
    void newDataService(const QString& unique_id,
                        const QString& direct_io_address,
                        int endpoint);
    void updateDataService(int current_row,
                           const QString& unique_id,
                           const QString& direct_io_address,
                           int endpoint);
private:
    Ui::CreateEditDataService *ui;
};

#endif // CREATEEDITDATASERVICE_H
