#include "CPopupWidgetContainer.h"
#include "ui_CPopupWidgetContainer.h"

CPopupWidgetContainer::CPopupWidgetContainer(QWidget *custom_widget, QWidget *parent) :
    QWidget(parent, Qt::Popup),
    ui(new Ui::CPopupWidgetContainer) {
    ui->setupUi(this);

    custom_widget->setParent(this);
    ui->widgetContainer->setLayout(new QHBoxLayout());
    ui->widgetContainer->layout()->addWidget(custom_widget);

    setWindowFlags( Qt::Window | Qt::FramelessWindowHint );
    setAttribute(Qt::WA_QuitOnClose, true);
    //pte->setAttribute( Qt::WA_TranslucentBackground );
    setWindowModality(Qt::WindowModal);
}

CPopupWidgetContainer::~CPopupWidgetContainer() {
    delete ui;
}

void CPopupWidgetContainer::on_pushButtonCancel_clicked() {
    close();
}
