#include "../../widget/designer/ui/CDatasetAttributeSet2VButton.h"
#include "CDatasetAttributeSet2VButtonPlugin.h"
#include "CDatasetAttributeLabelMenu.h"

#include <QtDesigner/QDesignerFormEditorInterface>
#include <QtDesigner/QExtensionManager>
#include <QtPlugin>

//! [0]
CDatasetAttributeSet2VButtonPlugin::CDatasetAttributeSet2VButtonPlugin(QObject *parent)
    : QObject(parent){}

QString CDatasetAttributeSet2VButtonPlugin::name() const {return QStringLiteral("CDatasetAttributeSet2VButton");}

QString CDatasetAttributeSet2VButtonPlugin::group() const {return QStringLiteral("Chaos UI");}

QString CDatasetAttributeSet2VButtonPlugin::toolTip() const {return QString();}

QString CDatasetAttributeSet2VButtonPlugin::whatsThis() const {return QString();}

QString CDatasetAttributeSet2VButtonPlugin::includeFile() const {return QStringLiteral("CDatasetAttributeSet2VButton.h");}

QIcon CDatasetAttributeSet2VButtonPlugin::icon() const {return QIcon();}

bool CDatasetAttributeSet2VButtonPlugin::isContainer() const {return false;}

bool CDatasetAttributeSet2VButtonPlugin::isInitialized() const { return initialized; }

QWidget *CDatasetAttributeSet2VButtonPlugin::createWidget(QWidget *parent)
{
    Q_INIT_RESOURCE(theme);
    CDatasetAttributeSet2VButton *chaos_widget = new CDatasetAttributeSet2VButton(parent);
    chaos_widget->chaosWidgetEditMode(true);
    return chaos_widget;
}

void CDatasetAttributeSet2VButtonPlugin::initialize(QDesignerFormEditorInterface *formEditor)
{
    if (initialized)
        return;

    QExtensionManager *manager = formEditor->extensionManager();
    Q_ASSERT(manager != nullptr);
    initialized = true;
}
