#ifndef SCRIPTSIGNALDIALOG_H
#define SCRIPTSIGNALDIALOG_H

#include <QHash>
#include <QDialog>
#include <QJSValue>
#include <QStringList>
#include <QMetaMethod>
#include <QSharedPointer>

namespace Ui {
class ScriptSignalDialog;
}

struct StructSignalFunction {
    QString signal_name;
    QString function_name;
    QString script_source;
};

class ScriptSignalDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ScriptSignalDialog(QWidget *_target_widget,
                                QWidget *parent = nullptr);
    ~ScriptSignalDialog();

    QMap<QString, QSharedPointer<StructSignalFunction> > getScript();
private slots:
    void on_comboBoxSignals_currentIndexChanged(const QString &method_name);

    void on_pushButtonSaveScript_clicked();
private:
    QWidget *target_widget;
    QMap<QString, QSharedPointer<StructSignalFunction>> hashSignalScript;
    Ui::ScriptSignalDialog *ui;
    QStringList getSignal(QWidget *wgt);
    QString defineStubForSignal(const QString& function_name,
                                QMetaMethod &method);
    inline QString normalizedMethodName(const QString& method_name);
    QString scriptFunctionNameForSignal(const QString& method_name);
};

#endif // SCRIPTSIGNALDIALOG_H
