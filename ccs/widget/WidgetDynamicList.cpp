#include "WidgetDynamicList.h"
#include <QVBoxLayout>

WidgetDynamicList::WidgetDynamicList(QWidget *parent):
    QWidget(parent){
    setLayout(new QVBoxLayout(this));
    ((QVBoxLayout*)layout())->insertStretch(-1);
    ((QVBoxLayout*)layout())->setContentsMargins(2,2,2,2);
}

void WidgetDynamicList::addWidget(QWidget *child) {
    int element_count = layout()->count();
    layout()->removeItem(layout()->itemAt(element_count-1));
    layout()->addWidget(child);
    ((QVBoxLayout*)layout())->insertStretch(element_count);
}

void WidgetDynamicList::removeWidgetByName(const QString& widget_name) {
    QWidget * child = layout()->findChild<QWidget*>(widget_name);
    if(child) {
        layout()->removeWidget(child);
    }
}
