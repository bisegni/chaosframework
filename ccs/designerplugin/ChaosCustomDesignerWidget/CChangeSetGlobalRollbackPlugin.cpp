#include "../../widget/designer/ui/CChangeSetGlobalRollback.h"
#include "CChangeSetGlobalRollbackPlugin.h"

#include "CDatasetAttributeHealthLabelPlugin.h"

#include <QtDesigner/QDesignerFormEditorInterface>
#include <QtDesigner/QExtensionManager>
#include <QtPlugin>

//! [0]
CChangeSetGlobalRollbackPlugin::CChangeSetGlobalRollbackPlugin(QObject *parent)
    : QObject(parent){}

QString CChangeSetGlobalRollbackPlugin::name() const {return QStringLiteral("CChangeSetGlobalRollback");}

QString CChangeSetGlobalRollbackPlugin::group() const {return QStringLiteral("Chaos UI");}

QString CChangeSetGlobalRollbackPlugin::toolTip() const {return QString();}

QString CChangeSetGlobalRollbackPlugin::whatsThis() const {return QString();}

QString CChangeSetGlobalRollbackPlugin::includeFile() const {return QStringLiteral("CChangeSetGlobalRollback.h");}

QIcon CChangeSetGlobalRollbackPlugin::icon() const {return QIcon();}

bool CChangeSetGlobalRollbackPlugin::isContainer() const {return false;}

bool CChangeSetGlobalRollbackPlugin::isInitialized() const { return initialized; }

QWidget *CChangeSetGlobalRollbackPlugin::createWidget(QWidget *parent) {
    Q_INIT_RESOURCE(theme);
    CChangeSetGlobalRollback *c_widget = new CChangeSetGlobalRollback(parent);
    return c_widget;
}

void CChangeSetGlobalRollbackPlugin::initialize(QDesignerFormEditorInterface *formEditor) {
    if (initialized)
        return;

    QExtensionManager *manager = formEditor->extensionManager();
    Q_ASSERT(manager != nullptr);

    initialized = true;
}
