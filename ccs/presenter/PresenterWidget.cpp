#include "PresenterWidget.h"
#include "CommandPresenter.h"
#include <QDebug>
#include <QCloseEvent>
#include <QMessageBox>
#include <QDebug>

using namespace chaos;
using namespace chaos::common::data;
using namespace chaos::metadata_service_client::api_proxy;

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

void PresenterWidget::addWidgetToPresenter(PresenterWidget *p_w) {
    assert(presenter_instance);
    presenter_instance->showCommandPresenter(p_w);
}

void PresenterWidget::submitApiResult(const QString& api_tag,
                                      ApiProxyResult& api_result) {
    api_processor.submitApiResult(api_tag,
                                  api_result,
                                  this,
                                  SLOT(asyncApiResult(QString, QSharedPointer<chaos::common::data::CDataWrapper>)),
                                  SLOT(asyncApiError(QString, QSharedPointer<chaos::CException>)),
                                  SLOT(asyncApiTimeout(QString)));
    //emit signal that we are starting the wait
    emit onStartWaitApi(api_tag);
}

void PresenterWidget::asyncApiResult(QString tag,
                                     QSharedPointer<chaos::common::data::CDataWrapper> api_result) {
    onApiDone(tag,
              api_result);
    //emit signal that we are finisched the wait on api result
    emit onEndWaitApi(tag);
}

void PresenterWidget::asyncApiError(QString tag,
                                    QSharedPointer<chaos::CException> api_exception) {
    onApiError(tag,
               api_exception);
    //emit signal that we are finisched the wait on api result
    emit onEndWaitApi(tag);
}

void PresenterWidget::asyncApiTimeout(QString tag) {
    onApiTimeout(tag);
}

void PresenterWidget::onApiDone(QString tag,
                                QSharedPointer<CDataWrapper> api_result) {
    qDebug() << "onApiDone event of tag:" << tag << " of error:" << QString::fromStdString(api_result->getJSONString());
}

void PresenterWidget::onApiError(QString tag,
                                 QSharedPointer<CException> api_exception) {
    qDebug() << "onApiError event of tag:" << tag << " of error:" << api_exception->what();
    QString title(tr("Api Error "));
    title.append("[");
    title.append(tag);
    title.append("]");
    QMessageBox::information(this,
                             tr("Api Error"),
                             api_exception->what());
}

void PresenterWidget::onApiTimeout(QString tag) {
    qDebug() << "onApiTimeout event of tag:" << tag;
    QString title(tr("Api Error"));
    title.append("[");
    title.append(tag);
    title.append("]");
    QMessageBox::information(this,
                             title,
                             tr("Timeout reached (Possible no server available)!"));
}
