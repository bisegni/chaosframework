#include "NewSnapshot.h"
#include "ui_NewSnapshot.h"

static QString TAG_SELECT_SNAPSHOT_NODE = "t_s_sn";
static QString TAG_API_NEW_SNAPSHOT = "t_a_ns";
using namespace chaos::metadata_service_client;
using namespace chaos::metadata_service_client::api_proxy;

NewSnapshot::NewSnapshot(QWidget *parent) :
    PresenterWidget(parent),
    search_node(NULL),
    ui(new Ui::NewSnapshot) {
    ui->setupUi(this);
}

NewSnapshot::~NewSnapshot() {
    delete ui;
}


void NewSnapshot::initUI() {
    setWindowTitle(tr("New Snapshot"));
    ui->listViewIncludedNode->setModel(&node_model);
    ui->pushButtonRemoveNode->setEnabled(false);

    //connect the list update selection signal
    connect(ui->listViewIncludedNode->selectionModel(),
            SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
            SLOT(selectionChanged(QItemSelection,QItemSelection)));
}

bool NewSnapshot::isClosing() {
    if(search_node)search_node->closeTab();
    return true;
}

void NewSnapshot::onApiDone(const QString& tag,
                            QSharedPointer<chaos::common::data::CDataWrapper> api_result) {
    PresenterWidget::onApiDone(tag,
                               api_result);
    closeTab();
}

void NewSnapshot::selectedNodes(const QString& tag,
                                const QVector<QPair<QString,QString> >& selected_nodes) {
    if(tag.compare(TAG_SELECT_SNAPSHOT_NODE) == 0) {
        QVectorIterator<QPair<QString,QString> > it(selected_nodes);
        while(it.hasNext()) {
            node_model.addNode(it.next().first);
        }
    }
}

void NewSnapshot::selectionChanged(const QItemSelection& start, const QItemSelection& end) {
    ui->pushButtonRemoveNode->setEnabled(ui->listViewIncludedNode->selectionModel()->selectedRows().size());
}

void NewSnapshot::destroyed(QObject *destroyed_search_node) {
    if(search_node == destroyed_search_node) {
        search_node = NULL;
    }
}

void NewSnapshot::on_pushButtonAddNode_clicked() {
    if((search_node=new SearchNodeResult(true, TAG_SELECT_SNAPSHOT_NODE))){
        connect(search_node,
                SIGNAL(selectedNodes(QString,QVector<QPair<QString,QString> >)),
                SLOT(selectedNodes(QString,QVector<QPair<QString,QString> >)));
        connect(search_node,
                SIGNAL(destroyed(QObject*)),
                SLOT(destroyed(QObject*)));
        addWidgetToPresenter(search_node);
    }
}

void NewSnapshot::on_pushButtonRemoveNode_clicked() {
    foreach (QModelIndex index, ui->listViewIncludedNode->selectionModel()->selectedRows()) {
        QString to_erase = node_model.getAllNode()[index.row()];
        node_model.removeNode(to_erase);
    }
}

void NewSnapshot::on_pushButtonCreateSnapshot_clicked() {
    service::SnapshotNodeList node_to_snap;
    QVectorIterator<QString> node_in_model_it(node_model.getAllNode());
    while(node_in_model_it.hasNext()) {
        node_to_snap.push_back(node_in_model_it.next().toStdString());
    }
    submitApiResult(TAG_API_NEW_SNAPSHOT,
                    GET_CHAOS_API_PTR(service::CreateNewSnapshot)->execute(ui->lineEditSnapshotName->text().toStdString(),
                                                                           node_to_snap));
}
