
#include "../../widget/designer/ui/CUOnlineLedIndicator.h"
#include "CUOnlineLedIndicatorPlugin.h"

#include <QtDesigner/QDesignerFormEditorInterface>
#include <QtDesigner/QExtensionManager>
#include <QtPlugin>

CUOnlineLedIndicatorPlugin::CUOnlineLedIndicatorPlugin(QObject *parent)
    : QObject(parent){}


QString CUOnlineLedIndicatorPlugin::name() const {return QStringLiteral("CUOnlineLedIndicator");}

QString CUOnlineLedIndicatorPlugin::group() const {return QStringLiteral("Chaos UI");}

QString CUOnlineLedIndicatorPlugin::toolTip() const {return QString();}

QString CUOnlineLedIndicatorPlugin::whatsThis() const {return QString();}

QString CUOnlineLedIndicatorPlugin::includeFile() const {return QStringLiteral("CUOnlineLedIndicator.h");}

QIcon CUOnlineLedIndicatorPlugin::icon() const {return QIcon();}

bool CUOnlineLedIndicatorPlugin::isContainer() const {return false;}

bool CUOnlineLedIndicatorPlugin::isInitialized() const { return initialized; }

QWidget *CUOnlineLedIndicatorPlugin::createWidget(QWidget *parent)
{
    CUOnlineLedIndicator *cuIndicator = new CUOnlineLedIndicator(parent);
    return cuIndicator;
}

void CUOnlineLedIndicatorPlugin::initialize(QDesignerFormEditorInterface *formEditor)
{
    if (initialized)
        return;

    QExtensionManager *manager = formEditor->extensionManager();
    Q_ASSERT(manager != nullptr);

//    manager->registerExtensions(new CDatasetAttributeLabelMenuFactory(manager),
//                                Q_TYPEID(QDesignerTaskMenuExtension));

    initialized = true;
}

QString CUOnlineLedIndicatorPlugin::domXml() const
{
    return QLatin1String("\
<ui language=\"c++\">\
    <widget class=\"CUOnlineLedIndicator\" name=\"cCUOnlineLedIndicator\"/>\
    <customwidgets>\
        <customwidget>\
            <class>CUOnlineLedIndicator</class>\
            <propertyspecifications>\
            <tooltip name=\"deviceID\">Device id</tooltip>\
            </propertyspecifications>\
        </customwidget>\
    </customwidgets>\
</ui>");
}

//! [3]
