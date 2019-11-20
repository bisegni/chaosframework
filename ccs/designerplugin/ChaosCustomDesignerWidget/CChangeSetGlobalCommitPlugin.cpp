#include "../../widget/designer/ui/CChangeSetGlobalCommit.h"
#include "CChangeSetGlobalCommitPlugin.h"

#include "CDatasetAttributeHealthLabelPlugin.h"

#include <QtDesigner/QDesignerFormEditorInterface>
#include <QtDesigner/QExtensionManager>
#include <QtPlugin>

//! [0]
CChangeSetGlobalCommitPlugin::CChangeSetGlobalCommitPlugin(QObject *parent)
    : QObject(parent){}

QString CChangeSetGlobalCommitPlugin::name() const {return QStringLiteral("CChangeSetGlobalCommit");}

QString CChangeSetGlobalCommitPlugin::group() const {return QStringLiteral("Chaos UI");}

QString CChangeSetGlobalCommitPlugin::toolTip() const {return QString();}

QString CChangeSetGlobalCommitPlugin::whatsThis() const {return QString();}

QString CChangeSetGlobalCommitPlugin::includeFile() const {return QStringLiteral("CChangeSetGlobalCommit.h");}

QIcon CChangeSetGlobalCommitPlugin::icon() const {return QIcon();}

bool CChangeSetGlobalCommitPlugin::isContainer() const {return false;}

bool CChangeSetGlobalCommitPlugin::isInitialized() const { return initialized; }

QWidget *CChangeSetGlobalCommitPlugin::createWidget(QWidget *parent) {
    Q_INIT_RESOURCE(theme);
    CChangeSetGlobalCommit *c_widget = new CChangeSetGlobalCommit(parent);
    return c_widget;
}

void CChangeSetGlobalCommitPlugin::initialize(QDesignerFormEditorInterface *formEditor) {
    if (initialized)
        return;

    QExtensionManager *manager = formEditor->extensionManager();
    Q_ASSERT(manager != nullptr);

    initialized = true;
}
