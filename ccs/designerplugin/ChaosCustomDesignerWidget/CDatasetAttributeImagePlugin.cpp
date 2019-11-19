#include "CDatasetAttributeImagePlugin.h"
#include "../../widget/designer/ui/CDatasetAttributeImage.h"
#include <QtDesigner/QDesignerFormEditorInterface>
#include <QtDesigner/QExtensionManager>
#include <QtPlugin>


//! [0]
CDatasetAttributeImagePlugin::CDatasetAttributeImagePlugin(QObject *parent)
    : QObject(parent){}

QString CDatasetAttributeImagePlugin::name() const {return QStringLiteral("CDatasetAttributeImage");}

QString CDatasetAttributeImagePlugin::group() const {return QStringLiteral("Chaos UI");}

QString CDatasetAttributeImagePlugin::toolTip() const {return QString();}

QString CDatasetAttributeImagePlugin::whatsThis() const {return QString();}

QString CDatasetAttributeImagePlugin::includeFile() const {return QStringLiteral("CDatasetAttributeImage.h");}

QIcon CDatasetAttributeImagePlugin::icon() const {return QIcon();}

bool CDatasetAttributeImagePlugin::isContainer() const {return false;}

bool CDatasetAttributeImagePlugin::isInitialized() const { return initialized; }

QWidget *CDatasetAttributeImagePlugin::createWidget(QWidget *parent)
{
    Q_INIT_RESOURCE(theme);
    CDatasetAttributeImage *chaos_widget = new CDatasetAttributeImage(parent);
    chaos_widget->chaosWidgetEditMode(true);
    return chaos_widget;
}

void CDatasetAttributeImagePlugin::initialize(QDesignerFormEditorInterface *formEditor)
{
    if (initialized)
        return;
    QExtensionManager *manager = formEditor->extensionManager();
    Q_ASSERT(manager != nullptr);
    initialized = true;
}

QString CDatasetAttributeImagePlugin::domXml() const
{
    return QLatin1String("\
<ui language=\"c++\">\
    <widget class=\"CDatasetAttributeImage\" name=\"cDatasetAttributeImage\"/>\
    <customwidgets>\
        <customwidget>\
            <class>CDatasetAttributeImage</class>\
            <propertyspecifications>\
            <tooltip name=\"deviceID\">Device id</tooltip>\
            </propertyspecifications>\
        </customwidget>\
    </customwidgets>\
</ui>");
}
