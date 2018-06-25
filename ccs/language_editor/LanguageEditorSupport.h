#ifndef LANGUAGEEDITORSUPPORT_H
#define LANGUAGEEDITORSUPPORT_H

#include <QString>
#include <QTextDocument>
#include <QSyntaxHighlighter>

class LanguageEditorSupport {
public:
    typedef enum LanguageHandler{
        ChaosWrapper,
        LaunchHandler,
        StartHandler,
        StepHandler,
        StopHandler,
        TerminateHandler,
        AttributeChangedHHandler,
    } LanguageHandler;

    LanguageEditorSupport(const QString& _description,
                          const QString& _vm_prefix):
    description(_description),
    vm_prefix(_vm_prefix){}

    virtual ~LanguageEditorSupport(){}

    const QString& getDescription() {return description;}

    const QString& getVMPrefix()  {return vm_prefix;}

    virtual QString getCodeForHandler(LanguageHandler handler_type) = 0;

    virtual QSyntaxHighlighter* getHiglighter(QTextDocument *parent) = 0;

private:
    const QString description;
    const QString vm_prefix;
};

#endif // LANGUAGEEDITORSUPPORT_H
