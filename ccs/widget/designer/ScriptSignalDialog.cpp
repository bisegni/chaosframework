#include "ScriptSignalDialog.h"
#include "ui_ScriptSignalDialog.h"

#include <QDebug>

ScriptSignalDialog::ScriptSignalDialog(QWidget *_target_widget, QWidget *parent) :
    QDialog(parent),
    target_widget(_target_widget),
    ui(new Ui::ScriptSignalDialog) {
    ui->setupUi(this);
    ui->comboBoxSignals->addItems(getSignal(target_widget));
}

ScriptSignalDialog::~ScriptSignalDialog() {
    delete ui;
}

QMap<QString, QSharedPointer<StructSignalFunction> > ScriptSignalDialog::getScript() {
    return hashSignalScript;
}

QStringList ScriptSignalDialog::getSignal(QWidget *wgt) {
    QStringList signalSignatures;
    if(wgt == nullptr) return signalSignatures;

    const QMetaObject *thisMeta = wgt->metaObject();
    for(int methodIdx = 0;
        methodIdx < thisMeta->methodCount();
        ++methodIdx) {
        QMetaMethod method = thisMeta->method(methodIdx);
        qDebug() << QString(method.methodSignature());
        switch(method.methodType()) {
        case QMetaMethod::Signal:
            signalSignatures.append(QString(method.methodSignature()));
            break;
        default:
            break;
        }
    }
    return signalSignatures;
}

QString ScriptSignalDialog::defineStubForSignal(const QString &function_name, QMetaMethod& method) {
    QString script_stub = QString("function %1 (").arg(function_name);
    for(int idx = 0;
        idx < method.parameterCount();
        idx++){
        QString param_name = QString(method.parameterNames()[idx]);
        if(param_name.size() == 0) {
            param_name = "param_" + QString::number(idx);
        }
        script_stub += param_name+",";
    }
    if(method.parameterCount()){script_stub.remove(script_stub.length()-1,1);}
    script_stub += "){\n"
                   "}";
    return script_stub;
}

void ScriptSignalDialog::on_comboBoxSignals_currentIndexChanged(const QString &method_name) {
    QString script_for_method;
    int m_idx = target_widget->metaObject()->indexOfMethod(method_name.toStdString().c_str());
    QMetaMethod method = target_widget->metaObject()->method(m_idx);
    //create method name for js function
    if(hashSignalScript.contains(normalizedMethodName(method_name)) == false) {
        script_for_method = defineStubForSignal(scriptFunctionNameForSignal(method_name),
                                                method);
    } else {
        script_for_method = hashSignalScript.find(normalizedMethodName(method_name)).value()->script_source;
    }
    ui->textEditJSEditor->setText(script_for_method);
}

void ScriptSignalDialog::on_pushButtonSaveScript_clicked() {
    QSharedPointer<StructSignalFunction> script_entry(new StructSignalFunction());
    script_entry->signal_name = normalizedMethodName(ui->comboBoxSignals->currentText());
    script_entry->function_name = scriptFunctionNameForSignal(ui->comboBoxSignals->currentText());
    script_entry->script_source = ui->textEditJSEditor->document()->toPlainText();

    hashSignalScript[normalizedMethodName(ui->comboBoxSignals->currentText())] = script_entry;
}

QString ScriptSignalDialog::normalizedMethodName(const QString& method_name) {
    return QString(method_name).remove(method_name.indexOf("("), method_name.lastIndexOf(")"));
}

QString ScriptSignalDialog::scriptFunctionNameForSignal(const QString& method_name) {
    return target_widget->objectName() +"_"+ normalizedMethodName(method_name);
}
