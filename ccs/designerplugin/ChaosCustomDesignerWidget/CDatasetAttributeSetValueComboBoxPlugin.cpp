#include "CDatasetAttributeSetValueComboBoxPlugin.h"

#include "../../widget/designer/ui/CDatasetAttributeSetValueComboBox.h"

#include "CDatasetAttributeSetValueComboBoxMenuExtension.h"

#include <QtDesigner/QDesignerFormEditorInterface>
#include <QtDesigner/QExtensionManager>
#include <QtPlugin>

//! [0]
CDatasetAttributeSetValueComboBoxPlugin::CDatasetAttributeSetValueComboBoxPlugin(QObject *parent)
    : QObject(parent){}

QString CDatasetAttributeSetValueComboBoxPlugin::name() const {return QStringLiteral("CDatasetAttributeSetValueComboBox");}

QString CDatasetAttributeSetValueComboBoxPlugin::group() const {return QStringLiteral("Chaos UI");}

QString CDatasetAttributeSetValueComboBoxPlugin::toolTip() const {return QString();}

QString CDatasetAttributeSetValueComboBoxPlugin::whatsThis() const {return QString();}

QString CDatasetAttributeSetValueComboBoxPlugin::includeFile() const {return QStringLiteral("CDatasetAttributeSetValueComboBox.h");}

QIcon CDatasetAttributeSetValueComboBoxPlugin::icon() const {return QIcon();}

bool CDatasetAttributeSetValueComboBoxPlugin::isContainer() const {return false;}

bool CDatasetAttributeSetValueComboBoxPlugin::isInitialized() const { return initialized; }

QWidget *CDatasetAttributeSetValueComboBoxPlugin::createWidget(QWidget *parent) {
    Q_INIT_RESOURCE(theme);
    CDatasetAttributeSetValueComboBox *chaos_widget = new CDatasetAttributeSetValueComboBox(parent);
    chaos_widget->chaosWidgetEditMode(true);
    return chaos_widget;
}

void CDatasetAttributeSetValueComboBoxPlugin::initialize(QDesignerFormEditorInterface *formEditor) {
    if (initialized)
        return;

    QExtensionManager *manager = formEditor->extensionManager();
    Q_ASSERT(manager != nullptr);

    manager->registerExtensions(new CDatasetAttributeSetValueComboBoxMenuFactory(manager),
                                Q_TYPEID(QDesignerTaskMenuExtension));


    initialized = true;
}

QString CDatasetAttributeSetValueComboBoxPlugin::domXml() const
{
    return QLatin1String("\
<ui language=\"c++\">\
    <widget class=\"CDatasetAttributeSetValueComboBox\" name=\"cDatasetAttributeSetValueComboBox\"/>\
    <customwidgets>\
        <customwidget>\
            <class>CDatasetAttributeSetValueComboBox</class>\
            <propertyspecifications>\
            <stringpropertyspecification name=\"setup\" notr=\"true\" type=\"multiline\"/>\
            </propertyspecifications>\
        </customwidget>\
    </customwidgets>\
</ui>");
}
