#ifndef LOGBROWSER_H
#define LOGBROWSER_H

#include "../presenter/PresenterWidget.h"
#include "../data/LogDataTableModel.h"
#include "../data/LogDomainListModel.h"
#include "../data/LogEntryAdvancedSearchTableModel.h"

#include <QWidget>

namespace Ui {
class LogBrowser;
}

class LogBrowser :
        public PresenterWidget {
    Q_OBJECT

public:
    explicit LogBrowser();
    ~LogBrowser();

protected:
    void initUI();
    bool isClosing();
    void updateAll();
private slots:
    void on_lineEditSearchText_editingFinished();

    void on_checkBoxSearchDate_clicked();

    void on_pushButtonStartSearch_clicked();

    void logEntriesTableSelectionChanged(const QModelIndex& current_selection,
                                         const QModelIndex& previous_selection);
private:
    Ui::LogBrowser *ui;
    LogDataTableModel log_data_table_model;
    LogDomainListModel log_domain_list_model;
    LogEntryAdvancedSearchTableModel log_entry_table_model;
};

#endif // LOGBROWSER_H
