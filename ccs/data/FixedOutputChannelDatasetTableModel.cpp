#include "FixedOutputChannelDatasetTableModel.h"

#include <QDateTime>

using namespace chaos::common::data;

FixedOutputChannelDatasetTableModel::FixedOutputChannelDatasetTableModel(const QString& node_uid,
                                                                         unsigned int dataset_type,
                                                                         QObject *parent):
    ChaosAbstractDataSetTableModel(node_uid,
                                   dataset_type,
                                   parent) {

}

FixedOutputChannelDatasetTableModel::~FixedOutputChannelDatasetTableModel() {

}

void FixedOutputChannelDatasetTableModel::updateData(const QSharedPointer<chaos::common::data::CDataWrapper>& _dataset) {
    //call superclas method taht will emit dataChagned
    data_wrapped = _dataset;
    int real_row = 0;
    if(data_wrapped.isNull() ||
            !data_wrapped->hasKey(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_DESCRIPTION)) return;
    beginResetModel();
    //reset all
    vector_doe.clear();
    map_doe_attribute_name_index.clear();
    //add the default timestamp field
    map_doe_attribute_name_index.insert(chaos::DataPackCommonKey::DPCK_TIMESTAMP,
                                        QSharedPointer<AttributeInfo>(new AttributeInfo(real_row++,
                                                                                        4,
                                                                                        chaos::DataType::TYPE_INT64)));
    map_doe_attribute_name_index.insert(chaos::DataPackCommonKey::DPCK_HIGH_RESOLUTION_TIMESTAMP,
                                        QSharedPointer<AttributeInfo>(new AttributeInfo(real_row++,
                                                                                        4,
                                                                                        chaos::DataType::TYPE_INT64)));
    map_doe_attribute_name_index.insert(chaos::ControlUnitDatapackCommonKey::RUN_ID,
                                        QSharedPointer<AttributeInfo>(new AttributeInfo(real_row++,
                                                                                        4,
                                                                                        chaos::DataType::TYPE_INT64)));
    QSharedPointer<CDataWrapper> element(new CDataWrapper());
    element->addStringValue(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_NAME, chaos::DataPackCommonKey::DPCK_TIMESTAMP);
    element->addInt32Value(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_TYPE, chaos::DataType::TYPE_INT64);
    element->addStringValue(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_DESCRIPTION, "DAQ timestamp");
    vector_doe.push_back(element);

    element.reset(new CDataWrapper());
    element->addStringValue(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_NAME, chaos::DataPackCommonKey::DPCK_HIGH_RESOLUTION_TIMESTAMP);
    element->addInt32Value(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_TYPE, chaos::DataType::TYPE_INT64);
    element->addStringValue(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_DESCRIPTION, "DAQ High resolution timestamp");
    vector_doe.push_back(element);

    element.reset(new CDataWrapper());
    element->addStringValue(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_NAME, chaos::ControlUnitDatapackCommonKey::RUN_ID);
    element->addInt32Value(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_TYPE, chaos::DataType::TYPE_INT64);
    element->addStringValue(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_DESCRIPTION, "Run identification");
    vector_doe.push_back(element);

    //add other output channels
    CMultiTypeDataArrayWrapperSPtr dataset_array = data_wrapped->getVectorValue(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_DESCRIPTION);
    for(int idx = 0;
        idx < dataset_array->size();
        idx++) {
        QSharedPointer<CDataWrapper> element(dataset_array->getCDataWrapperElementAtIndex(idx).release());
        if(element->hasKey(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_DESCRIPTION) &&
                element->hasKey(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_NAME) &&
                element->hasKey(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_TYPE) &&
                element->hasKey(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_DIRECTION)) {
            int direction = element->getInt32Value(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_DIRECTION);
            if(direction == chaos::DataType::Output ||
                    direction == chaos::DataType::Bidirectional) {
                //create index for the current value of this element
                map_doe_attribute_name_index.insert(QString::fromStdString(element->getStringValue(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_NAME)),
                                                    QSharedPointer<AttributeInfo>(new AttributeInfo(real_row++,
                                                                                                    4,
                                                                                                    (chaos::DataType::DataType)element->getInt32Value(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_TYPE))));
                vector_doe.push_back(element);
            }
        }
    }
    endResetModel();
}

int FixedOutputChannelDatasetTableModel::getRowCount() const {
    return vector_doe.size();
}

int FixedOutputChannelDatasetTableModel::getColumnCount() const {
    return monitoring_enabled?4:3;
}

QString FixedOutputChannelDatasetTableModel::getHeaderForColumn(int column) const {
    QString result;
    switch(column) {
    case 0:
        result = QString("Name");
        break;
    case 1:
        result = QString("Type");
        break;
    case 2:
        result = QString("Description");
        break;
    case 3:
        result = QString("Current value");
        break;
    }
    return result;
}

QVariant FixedOutputChannelDatasetTableModel::getCellData(int row, int column) const {
    QVariant result;
    QSharedPointer<CDataWrapper> element = vector_doe[row];
    switch(column) {
    case 0:
        if(element->hasKey(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_NAME)) {
            result = QString::fromStdString(element->getStringValue(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_NAME));
        }
        break;

    case 1:
        if(element->hasKey(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_TYPE)) {
            switch (element->getInt32Value(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_TYPE)) {
            case chaos::DataType::TYPE_BOOLEAN:
                result = QString("boolean");
                break;
            case chaos::DataType::TYPE_INT32:
                result = QString("Int32");
                break;
            case chaos::DataType::TYPE_INT64:
                result = QString("Int64");
                break;
            case chaos::DataType::TYPE_CLUSTER:
                result = QString("cluster");
                break;
            case chaos::DataType::TYPE_STRING:
                result = QString("String");
                break;
            case chaos::DataType::TYPE_DOUBLE:
                result = QString("Double");
                break;
            case chaos::DataType::TYPE_BYTEARRAY:
                result = QString("Binary");
                break;

            default:
                break;
            }
        }
        break;

    case 2:
        if(element->hasKey(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_DESCRIPTION)) {
            return QString::fromStdString(element->getStringValue(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_DESCRIPTION));
        }
        break;
    case 3:
        if(map_doe_current_values.find(row) == map_doe_current_values.end()) {
            result = QString("...");
        } else {
            result = map_doe_current_values[row];
            if(row == 0) {
                result = QDateTime::fromMSecsSinceEpoch(result.toLongLong(), Qt::LocalTime).toString();
            }
        }
        break;
    }
    return result;
}

QString FixedOutputChannelDatasetTableModel::getSubTypeForCode(int subtype) const {
    QString result = "...";
    bool is_unsigned = ((subtype&chaos::DataType::SUB_TYPE_UNSIGNED) == chaos::DataType::SUB_TYPE_UNSIGNED);
    int real_subtype = is_unsigned?(subtype^chaos::DataType::SUB_TYPE_UNSIGNED):subtype;
    switch (real_subtype) {
    case chaos::DataType::SUB_TYPE_BOOLEAN:
        result = QString("boolean");
        break;
    case chaos::DataType::SUB_TYPE_INT8:
        result = is_unsigned?QString("Int8"):QString("UInt8");
        break;
    case chaos::DataType::SUB_TYPE_INT16:
        result = is_unsigned?QString("Int16"):QString("UInt16");
        break;
    case chaos::DataType::SUB_TYPE_INT32:
        result = is_unsigned?QString("Int32"):QString("UInt32");
        break;
    case chaos::DataType::SUB_TYPE_INT64:
        result = is_unsigned?QString("Int64"):QString("UInt64");
        break;
    case chaos::DataType::SUB_TYPE_CHAR:
        result = QString("Char");
        break;
    case chaos::DataType::SUB_TYPE_DOUBLE:
        result = QString("Double");
        break;
    case chaos::DataType::SUB_TYPE_MIME:
        result = QString("MIME");
        break;
    case chaos::DataType::SUB_TYPE_STRING:
        result = QString("String");
        break;
    case chaos::DataType::SUB_TYPE_NONE:
        result = QString("MIME");
        break;
    default:
        break;
    }
    return result;
}

QVariant FixedOutputChannelDatasetTableModel::getTooltipTextForData(int row, int column) const {
    QVariant result;
    QSharedPointer<CDataWrapper> element = vector_doe[row];
    switch(column) {
    case 1:    //

        if(element->hasKey(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_TYPE)) {
            switch (element->getInt32Value(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_TYPE)) {
            case chaos::DataType::TYPE_BYTEARRAY:
                if(element->hasKey(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_BINARY_SUBTYPE)) {
                    int32_t cardinality = 1;
                    QString sub_type_desription;
                    if(element->hasKey(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_BINARY_CARDINALITY)) {
                        cardinality = element->getInt32Value(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_BINARY_CARDINALITY);
                    }

                    //append cardinality
                    sub_type_desription.append(QString("Cardinality:%1\n").arg(cardinality));

                    //we have a definition of subtype
                    if(element->isVectorValue(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_BINARY_SUBTYPE)) {
                        QString composed_subtype_desc;
                        //we a structure
                       CMultiTypeDataArrayWrapperSPtr sub_type_list = element->getVectorValue(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_BINARY_SUBTYPE);
                        for(int idx = 0;
                            idx < sub_type_list->size();
                            idx++) {
                            sub_type_desription.append(QString("- %1\n").arg(getSubTypeForCode(element->getInt32Value(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_BINARY_SUBTYPE))));
                        }
                    } else {
                        //we have a single subtype
                        sub_type_desription.append(QString("- %1\n").arg(getSubTypeForCode(element->getInt32Value(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_BINARY_SUBTYPE))));
                    }
                    sub_type_desription.resize(sub_type_desription.size()-1);
                    result = sub_type_desription;
                } else {
                    result = getCellData(row, column);
                }
                break;
            default:
                result = getCellData(row, column);
                break;
            }
        }
        break;

    default:
        result = getCellData(row, column);
        break;
    }
    return result;
}

QVariant FixedOutputChannelDatasetTableModel::getTextAlignForData(int row, int column) const {
    QVariant result;
    switch(column) {
    case 0:
    case 1:
    case 2:
        result =  Qt::AlignLeft+Qt::AlignVCenter;
        break;
    case 3:
        result =  Qt::AlignHCenter+Qt::AlignVCenter;
        break;
    }

    return result;
}
