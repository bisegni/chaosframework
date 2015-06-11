#include "CommandTemplateListModel.h"

using namespace chaos::common::data;
using namespace chaos::common::batch_command;

CommandTemplateListModel::CommandTemplateListModel(QObject *parent):
    ChaosAbstractListModel(parent){}

void CommandTemplateListModel::updateSearchPage(const QSharedPointer<CDataWrapper>& template_result_page) {
    beginResetModel();
    page_element.clear();
    if(!template_result_page->hasKey("cmd_tmpl_search_result_page")) return;

    QSharedPointer<CMultiTypeDataArrayWrapper> result_page(template_result_page->getVectorValue("cmd_tmpl_search_result_page"));
    for(int idx = 0;
        idx < result_page->size();
        idx++) {
        QSharedPointer<CDataWrapper> element_found(result_page->getCDataWrapperElementAtIndex(idx));
        page_element.push_back(element_found);
    }
    endResetModel();
}

int CommandTemplateListModel::getRowCount() const {
    return page_element.size();
}

QVariant CommandTemplateListModel::getUserData(int row) const {
    if(page_element.size()==0) return QVariant();
    return QString::fromStdString(page_element[row]->getStringValue(BatchCommandAndParameterDescriptionkey::BC_UNIQUE_ID));
}

QVariant CommandTemplateListModel::getRowData(int row) const {
    if(page_element.size()==0) return QVariant();
    return QVariant::fromValue(QString::fromStdString(page_element[row]->getStringValue("template_name")));
}
