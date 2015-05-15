#include "PresenterWidget.h"
#include "CommandPresenter.h"

#include <QMenu>
#include <QDebug>
#include <QCloseEvent>
#include <QMessageBox>

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

void PresenterWidget::addNodeToHealtMonitor(const QString& node) {
    assert(presenter_instance);
    presenter_instance->addMonitorHealtForNode(node);
}

void PresenterWidget::removeNodeToHealtMonitor(const QString& node) {
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

//---------------contextual menu utility------------
void PresenterWidget::registerWidgetForContextualMenu(QWidget *contextual_menu_parent) {
    if(!contextual_menu_parent) return;
    contextual_menu_parent->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(contextual_menu_parent, SIGNAL(customContextMenuRequested(QPoint)),
            this, SLOT(contextualMenuWillBeShown(QPoint)));
}

void PresenterWidget::contextualMenuWillBeShown(const QPoint& cm_start_point) {
    QWidget *child = this->childAt(cm_start_point);
    QMenu *menu = new QMenu(this);

    //let superclass to add custom action
    addCustomActionToContextualMenuForWidget(child, cm_start_point, menu);

    //show the contextual menu
    menu->popup(child->mapToGlobal(cm_start_point));
}

void PresenterWidget::addCustomActionToContextualMenuForWidget(QWidget *contextual_menu_parent,
                                                               const QPoint &cm_start_point,
                                                               QMenu *contextual_menu) {

}

void PresenterWidget::addDefaultNodeAction(QMenu *contextual_menu) {
    QAction *start_healt_monitoring_action = new QAction("Start healt monitor", contextual_menu);
    connect(start_healt_monitoring_action, SIGNAL(triggered()), this, SLOT(startHealtMonitorAction()));
    contextual_menu->addAction(start_healt_monitoring_action);

    QAction *stop_healt_monitoring_action = new QAction("Stop healt monitor", contextual_menu);
    connect(stop_healt_monitoring_action, SIGNAL(triggered()), this, SLOT(stopHealtMonitorAction()));
    contextual_menu->addAction(stop_healt_monitoring_action);
}

void PresenterWidget::startHealtMonitorAction() {

}

void PresenterWidget::stopHealtMonitorAction() {

}
