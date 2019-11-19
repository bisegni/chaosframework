#include "../../widget/designer/ui/CDatasetAttributeHealthLabel.h"
#include "CDatasetAttributeHealthLabelPlugin.h"

#include <QtDesigner/QDesignerFormEditorInterface>
#include <QtDesigner/QExtensionManager>
#include <QtPlugin>

//! [0]
CDatasetAttributeHealthLabelPlugin::CDatasetAttributeHealthLabelPlugin(QObject *parent)
    : QObject(parent){}

QString CDatasetAttributeHealthLabelPlugin::name() const {return QStringLiteral("CDatasetAttributeHealthLabel");}

QString CDatasetAttributeHealthLabelPlugin::group() const {return QStringLiteral("Chaos UI");}

QString CDatasetAttributeHealthLabelPlugin::toolTip() const {return QString();}

QString CDatasetAttributeHealthLabelPlugin::whatsThis() const {return QString();}

QString CDatasetAttributeHealthLabelPlugin::includeFile() const {return QStringLiteral("CDatasetAttributeHealthLabel.h");}

QIcon CDatasetAttributeHealthLabelPlugin::icon() const {return QIcon();}

bool CDatasetAttributeHealthLabelPlugin::isContainer() const {return false;}

bool CDatasetAttributeHealthLabelPlugin::isInitialized() const { return initialized; }

QWidget *CDatasetAttributeHealthLabelPlugin::createWidget(QWidget *parent) {
    Q_INIT_RESOURCE(theme);
    CDatasetAttributeHealthLabel *chaos_widget = new CDatasetAttributeHealthLabel(parent);
    chaos_widget->chaosWidgetEditMode(true);
    return chaos_widget;
}

void CDatasetAttributeHealthLabelPlugin::initialize(QDesignerFormEditorInterface *formEditor) {
    if (initialized)
        return;

    QExtensionManager *manager = formEditor->extensionManager();
    Q_ASSERT(manager != nullptr);

    initialized = true;
}

QString CDatasetAttributeHealthLabelPlugin::domXml() const {
    return QLatin1String("\
<ui language=\"c++\">\
    <widget class=\"CDatasetAttributeHealthLabel\" name=\"cDatasetAttributeHealthLabel\"/>\
    <customwidgets>\
        <customwidget>\
            <class>CDatasetAttributeHealthLabel</class>\
            <propertyspecifications>\
            <tooltip name=\"deviceID\">Device id</tooltip>\
            </propertyspecifications>\
        </customwidget>\
    </customwidgets>\
</ui>");
}
