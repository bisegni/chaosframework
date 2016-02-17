#ifndef CNODELOGWIDGET_H
#define CNODELOGWIDGET_H

#include <QWidget>

#include <ChaosMetadataServiceClient/ChaosMetadataServiceClient.h>

namespace Ui {
class CNodeLogWidget;
}

class CNodeLogWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CNodeLogWidget(QWidget *parent = 0);
    ~CNodeLogWidget();

private:
    Ui::CNodeLogWidget *ui;
};

#endif // CNODELOGWIDGET_H
