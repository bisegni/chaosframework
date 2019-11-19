#include "CDatasetAttributeSetValueLineEditPlugin.h"

#include "../../widget/designer/ui/CDatasetAttributeSetValueLineEdit.h"

#include <QtDesigner/QDesignerFormEditorInterface>
#include <QtDesigner/QExtensionManager>
#include <QtPlugin>

//! [0]
CDatasetAttributeSetValueLineEditPlugin::CDatasetAttributeSetValueLineEditPlugin(QObject *parent)
    : QObject(parent){}

QString CDatasetAttributeSetValueLineEditPlugin::name() const {return QStringLiteral("CDatasetAttributeSetValueLineEdit");}

QString CDatasetAttributeSetValueLineEditPlugin::group() const {return QStringLiteral("Chaos UI");}

QString CDatasetAttributeSetValueLineEditPlugin::toolTip() const {return QString();}

QString CDatasetAttributeSetValueLineEditPlugin::whatsThis() const {return QString();}

QString CDatasetAttributeSetValueLineEditPlugin::includeFile() const {return QStringLiteral("CDatasetAttributeSetValueLineEditPlugin.h");}

QIcon CDatasetAttributeSetValueLineEditPlugin::icon() const {return QIcon();}

bool CDatasetAttributeSetValueLineEditPlugin::isContainer() const {return false;}

bool CDatasetAttributeSetValueLineEditPlugin::isInitialized() const { return initialized; }

QWidget *CDatasetAttributeSetValueLineEditPlugin::createWidget(QWidget *parent)
{
    Q_INIT_RESOURCE(theme);
    CDatasetAttributeSetValueLineEdit *chaos_widget = new CDatasetAttributeSetValueLineEdit(parent);

    return chaos_widget;
}

void CDatasetAttributeSetValueLineEditPlugin::initialize(QDesignerFormEditorInterface *formEditor)
{
    if (initialized)
        return;

    QExtensionManager *manager = formEditor->extensionManager();
    Q_ASSERT(manager != nullptr);

//    manager->registerExtensions(new CDatasetAttributeLabelMenuFactory(manager),
//                                Q_TYPEID(QDesignerTaskMenuExtension));

    initialized = true;
}

QString CDatasetAttributeSetValueLineEditPlugin::domXml() const
{
    return QLatin1String("\
<ui language=\"c++\">\
    <widget class=\"CDatasetAttributeSetValueLineEdit\" name=\"cDatasetAttributeSetValueLineEdit\"/>\
    <customwidgets>\
        <customwidget>\
            <class>CDatasetAttributeSetValueLineEdit</class>\
            <propertyspecifications>\
            <tooltip name=\"deviceID\">Device id</tooltip>\
            </propertyspecifications>\
        </customwidget>\
    </customwidgets>\
</ui>");
}
