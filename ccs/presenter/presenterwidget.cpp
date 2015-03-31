#include "PresenterWidget.h"
#include <QDebug>
#include <QCloseEvent>
PresenterWidget::PresenterWidget(QWidget *parent) :
    QWidget(parent),
    editor_subwindow(NULL)
{
     setAttribute(Qt::WA_DeleteOnClose);
}

PresenterWidget::~PresenterWidget()
{

}

void PresenterWidget::setSubWindow(QMdiSubWindow *_editor_subwindow) {
    if(editor_subwindow) return;
    editor_subwindow = _editor_subwindow;
}

void PresenterWidget::setTabTitle(const QString& title) {
    if(!editor_subwindow) return;
    editor_subwindow->setWindowTitle(title);
}

void PresenterWidget::closeEvent(QCloseEvent *event) {
    if(canClose()){
        event->accept();
    }
}
