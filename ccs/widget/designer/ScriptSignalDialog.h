#ifndef SCRIPTSIGNALDIALOG_H
#define SCRIPTSIGNALDIALOG_H

#include <QDialog>
#include <QStringList>

namespace Ui {
class ScriptSignalDialog;
}

class ScriptSignalDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ScriptSignalDialog(const QStringList& signal_list,
                                QWidget *parent = nullptr);
    ~ScriptSignalDialog();

private:
    Ui::ScriptSignalDialog *ui;
};

#endif // SCRIPTSIGNALDIALOG_H
