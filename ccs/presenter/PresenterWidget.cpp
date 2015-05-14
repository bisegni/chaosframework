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

void PresenterWidget::closeTab() {
    if(!editor_subwindow) return;
    editor_subwindow->close();
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

void PresenterWidget::addMonitorHealtForNode(const QString& node) {
    assert(presenter_instance);
    presenter_instance->addMonitorHealtForNode(node);
}

void PresenterWidget::removeMonitorHealtForNode(const QString& node) {
    assert(presenter_instance);
    presenter_instance->removeMonitorHealtForNode(node);
}

void PresenterWidget::submitApiResult(const QString& api_tag,
                                      ApiProxyResult api_result) {
    api_processor.submitApiResult(api_tag,
                                  api_result,
                                  this,
                                  SLOT(asyncApiResult(QString, QSharedPointer<chaos::common::data::CDataWrapper>)),
                                  SLOT(asyncApiError(QString, QSharedPointer<chaos::CException>)),
                                  SLOT(asyncApiTimeout(QString)));
    //emit signal that we are starting the wait
    emit onStartWaitApi(api_tag);
}

void PresenterWidget::asyncApiResult(const QString& tag,
                                     QSharedPointer<chaos::common::data::CDataWrapper> api_result) {
    onApiDone(tag,
              api_result);
    //emit signal that we are finisched the wait on api result
    emit onEndWaitApi(tag);
}

void PresenterWidget::asyncApiError(const QString& tag,
                                    QSharedPointer<chaos::CException> api_exception) {
    onApiError(tag,
               api_exception);
    //emit signal that we are finisched the wait on api result
    emit onEndWaitApi(tag);
}

void PresenterWidget::asyncApiTimeout(const QString& tag) {
    onApiTimeout(tag);
    emit onEndWaitApi(tag);
}

void PresenterWidget::onApiDone(const QString& tag,
                                QSharedPointer<CDataWrapper> api_result) {
    qDebug() << "onApiDone event of tag:" << tag << " of error:" << QString::fromStdString(api_result->getJSONString());
    showInformation(tr("Api Error"),
                    tag,
                    "Success");
}

void PresenterWidget::onApiError(const QString& tag,
                                 QSharedPointer<CException> api_exception) {
    qDebug() << "onApiError event of tag:" << tag << " of error:" << api_exception->what();
    showInformation(tr("Api Error"),
                    tag,
                    api_exception->what());
}

void PresenterWidget::onApiTimeout(const QString& tag) {
    qDebug() << "onApiTimeout event of tag:" << tag;
    showInformation(tr("Api Error"),
                    tag,
                    tr("Timeout reached (Possible no server available)!"));
}

 void PresenterWidget::showInformation(const QString& title,
                                       const QString& sub_title,
                                       const QString& information) {
     QString title_(title);
     title_.append("[");
     title_.append(sub_title);
     title_.append("]");
     QMessageBox::information(this,
                              title_,
                              information);
 }
