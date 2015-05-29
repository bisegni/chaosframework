#ifndef FIXEDINPUTCHANNELDATASETTABLEMODEL_H
#define FIXEDINPUTCHANNELDATASETTABLEMODEL_H

#include "ChaosAbstractDataSetTableModel.h"

#include <chaos/common/chaos_constants.h>
#include <chaos/common/data/CDataWrapper.h>

#include <boost/dynamic_bitset.hpp>

#include <QVector>
#include <QSharedPointer>

struct ChangeValue{
    QVariant last_value;
    QVariant current_value;

    void setCurrentValue(const QVariant& cur_val);
    void reset();
    void commit();
};

class FixedInputChannelDatasetTableModel:
        public ChaosAbstractDataSetTableModel
{
    Q_OBJECT
public:
    FixedInputChannelDatasetTableModel(const QString& node_uid,
                                       unsigned int dataset_type,
                                       QObject *parent = 0);
    ~FixedInputChannelDatasetTableModel();
    void updateData(const QSharedPointer<chaos::common::data::CDataWrapper>& _dataset);
    void updateInstanceDescription(const QSharedPointer<chaos::common::data::CDataWrapper>& _dataset_attribute_configuration);
    //! return all changed value and reset the tracking for new chagnes
    void getAttributeChangeSet(std::vector<boost::shared_ptr<chaos::metadata_service_client::api_proxy::control_unit::ControlUnitInputDatasetChangeSet> > &value_set_array);
    void applyChangeSet(bool commit);
protected:
    int getRowCount() const;
    int getColumnCount() const;
    QString getHeaderForColumn(int column) const;
    QVariant getCellData(int row, int column) const;
    QVariant getTooltipTextForData(int row, int column) const;
    QVariant getTextAlignForData(int row, int column) const;
    QVariant getBackgroundForData(int row, int column) const;
    QVariant getTextColorForData(int row, int column) const;
    bool setCellData(const QModelIndex &index, const QVariant &value);
    bool isCellEditable(const QModelIndex &index) const;
private:
    //memorize all the attributes that have had their values changed
    boost::dynamic_bitset<> attribute_value_changed;
    //contains the value for set
    QVector< QSharedPointer<ChangeValue> > attribute_set_value;
    QMap<int, QSharedPointer<chaos::common::data::CDataWrapper> >dataset_attribute_configuration;
};

#endif // FIXEDINPUTCHANNELDATASETTABLEMODEL_H
