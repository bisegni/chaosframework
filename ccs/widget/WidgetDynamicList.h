#ifndef WIDGETDYNAMICLIST_H
#define WIDGETDYNAMICLIST_H

#include <QWidget>

class WidgetDynamicList:
        public QWidget
{
public:
    WidgetDynamicList(QWidget *parent = 0);
    void addWidget(QWidget *child);
    void removeWidgetByName(const QString &widget_name);
};

#endif // WIDGETDYNAMICLIST_H
