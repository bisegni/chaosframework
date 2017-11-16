#include "CPopupWidgetContainer.h"
#include "ui_CPopupWidgetContainer.h"

CPopupWidgetContainer::CPopupWidgetContainer(QWidget *parent) :
    QWidget(parent, Qt::Popup),
    ui(new Ui::CPopupWidgetContainer) {
    ui->setupUi(this);
    ui->widgetContainer->setLayout(new QVBoxLayout());
    setWindowFlags( Qt::Window | Qt::FramelessWindowHint );
    setAttribute(Qt::WA_QuitOnClose, true);
}

CPopupWidgetContainer::~CPopupWidgetContainer() {
    delete ui;
}

void CPopupWidgetContainer::addWidget(QWidget *custom_widget) {
    custom_widget->setParent(this);
    ui->widgetContainer->layout()->addWidget(custom_widget);
}

void CPopupWidgetContainer::on_pushButtonCancel_clicked() {
    close();
}
