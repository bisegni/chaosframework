#include "PresenterWidget.h"
#include "CommandPresenter.h"

#include <QMenu>
#include <QDebug>
#include <QCloseEvent>
#include <QMessageBox>

using namespace chaos;
using namespace chaos::common::data;
using namespace chaos::metadata_service_client;
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
    if(isClosing()){
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

void PresenterWidget::manageWidgetStateOnForValue(const QString& value) {
    QList<QWidget *> w_list = findChildren<QWidget*>();
    foreach (QWidget* wdg, w_list) {
        QVariant state_on_value = wdg->property("_state_on");
        if(!state_on_value.isNull()) {
            wdg->setEnabled(state_on_value.toString().contains(value));
        }
    }
}

QString PresenterWidget::getDatasetKeyFromNodeKey(const QString& node_key,
                                                    unsigned int dataset_type) {
    QString result = QString::fromStdString(ChaosMetadataServiceClient::getInstance()->getDatasetKeyFromGeneralKey(node_key.toStdString(),
                                                                                                                   dataset_type));
    return result;
}

QString PresenterWidget::getHealttKeyFromNodeKey(const QString& node_key) {
    QString result = QString::fromStdString(ChaosMetadataServiceClient::getInstance()->getHealtKeyFromGeneralKey(node_key.toStdString()));
    return result;
}

void PresenterWidget::registerMonitorHandler(const QString& monitor_key,
                                             const unsigned int dataset_type,
                                             unsigned int quantum_multiplier,
                                             AbstractAttributeHandler *monitor_attribute_handler) {
    registerMonitorHandler(monitor_key.toStdString(),
                           dataset_type,
                           quantum_multiplier,
                           monitor_attribute_handler);
}

void PresenterWidget::registerMonitorHandler(const std::string& monitor_key,
                                             const unsigned int dataset_type,
                                             unsigned int quantum_multiplier,
                                             AbstractAttributeHandler *monitor_attribute_handler) {
    //connect monitor signal to local virtual slot
        connect(monitor_attribute_handler,
                SIGNAL(valueUpdated(QString,QString,QVariant)),
                SLOT(monitorHandlerUpdateAttributeValue(QString,QString,QVariant)));

    //register the monitor
    ChaosMetadataServiceClient::getInstance()->addKeyAttributeHandlerForDataset(monitor_key,
                                                                                dataset_type,
                                                                                quantum_multiplier,
                                                                                monitor_attribute_handler->getQuantumAttributeHandler());
}

void PresenterWidget::unregisterMonitorHandler(const QString& monitor_key,
                                               const unsigned int dataset_type,
                                               unsigned int quantum_multiplier,
                                               AbstractAttributeHandler *monitor_attribute_handler) {
    unregisterMonitorHandler(monitor_key.toStdString(),
                             dataset_type,
                             quantum_multiplier,
                             monitor_attribute_handler);
}


void PresenterWidget::unregisterMonitorHandler(const std::string& monitor_key,
                                               const unsigned int dataset_type,
                                               unsigned int quantum_multiplier,
                                               AbstractAttributeHandler *monitor_attribute_handler) {
    //deregister the node
    ChaosMetadataServiceClient::getInstance()->removeKeyAttributeHandlerForDataset(monitor_key,
                                                                                   dataset_type,
                                                                                   quantum_multiplier,
                                                                                   monitor_attribute_handler->getQuantumAttributeHandler());
}
void PresenterWidget::registerHealtMonitorHandler(const QString& monitor_key,
                                                  unsigned int quantum_multiplier,
                                                  AbstractAttributeHandler *monitor_attribute_handler){
    registerHealtMonitorHandler(monitor_key.toStdString(),
                                quantum_multiplier,
                                monitor_attribute_handler);
}

void PresenterWidget::unregisterHealtMonitorHandler(const QString& monitor_key,
                                                    unsigned int quantum_multiplier,
                                                    AbstractAttributeHandler *monitor_attribute_handler) {
    unregisterHealtMonitorHandler(monitor_key.toStdString(),
                                  quantum_multiplier,
                                  monitor_attribute_handler);
}

void PresenterWidget::registerHealtMonitorHandler(const std::string& monitor_key,
                                                  unsigned int quantum_multiplier,
                                                  AbstractAttributeHandler *monitor_attribute_handler) {
        connect(monitor_attribute_handler,
                SIGNAL(valueUpdated(QString,QString,QVariant)),
                SLOT(monitorHandlerUpdateAttributeValue(QString,QString,QVariant)));

    //register the monitor
    ChaosMetadataServiceClient::getInstance()->addKeyAttributeHandlerForHealt(monitor_key,
                                                                              quantum_multiplier,
                                                                              monitor_attribute_handler->getQuantumAttributeHandler());
}

void PresenterWidget::unregisterHealtMonitorHandler(const std::string& monitor_key,
                                                    unsigned int quantum_multiplier,
                                                    AbstractAttributeHandler *monitor_attribute_handler) {
    //deregister the node
    ChaosMetadataServiceClient::getInstance()->removeKeyAttributeHandlerForHealt(monitor_key,
                                                                                 quantum_multiplier ,
                                                                                 monitor_attribute_handler->getQuantumAttributeHandler());

}

void PresenterWidget::monitorHandlerUpdateAttributeValue(const QString& key,
                                                         const QString& attribute_name,
                                                         const QVariant& attribute_value) {
#pragma unused (key)
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

//-------slot for api-------
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
void PresenterWidget::registerWidgetForContextualMenu(QWidget *contextual_menu_parent,
                                                      QMap<QString, QVariant> *widget_contextual_menu_action,
                                                      bool add_default_node_action) {
    if(!contextual_menu_parent) return;
    contextual_menu_parent->setContextMenuPolicy(Qt::ActionsContextMenu);

    if(widget_contextual_menu_action){
        QMapIterator<QString, QVariant> it(*widget_contextual_menu_action);
        while(it.hasNext()) {
            QAction *action = new QAction(it.key(), contextual_menu_parent);
            action->setData(it.value());
            contextual_menu_parent->addAction(action);
            connect(action,
                    SIGNAL(triggered()),
                    this,
                    SLOT(generalContextualMenuActionTrigger()));
        }
    }
    if(add_default_node_action)addDefaultNodeAction(contextual_menu_parent);
}


void PresenterWidget::addDefaultNodeAction(QWidget *contextual_menu_parent) {
    QAction *start_healt_monitoring_action = new QAction("Start healt monitor", contextual_menu_parent);
    connect(start_healt_monitoring_action, SIGNAL(triggered()), this, SLOT(startHealtMonitorAction()));
    contextual_menu_parent->addAction(start_healt_monitoring_action);

    QAction *stop_healt_monitoring_action = new QAction("Stop healt monitor", contextual_menu_parent);
    connect(stop_healt_monitoring_action, SIGNAL(triggered()), this, SLOT(stopHealtMonitorAction()));
    contextual_menu_parent->addAction(stop_healt_monitoring_action);
}

void PresenterWidget::generalContextualMenuActionTrigger() {
    QAction* cm_action = qobject_cast<QAction*>(sender());
    contextualMenuActionTrigger(cm_action->text(), cm_action->data());
}

void PresenterWidget::contextualMenuActionTrigger(const QString& cm_title,
                                                  const QVariant& cm_data){

}

void PresenterWidget::startHealtMonitorAction() {

}

void PresenterWidget::stopHealtMonitorAction() {

}
