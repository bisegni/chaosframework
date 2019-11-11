#ifndef FIXEDINPUTCHANNELDATASETTABLEMODEL_H
#define FIXEDINPUTCHANNELDATASETTABLEMODEL_H

#include "AttributeValueChangeSet.h"
#include "ChaosAbstractDataSetTableModel.h"
#include <boost/dynamic_bitset.hpp>

#include <QVector>
#include <QSharedPointer>


class FixedInputChannelDatasetTableModel:
        public ChaosAbstractDataSetTableModel
{
    Q_OBJECT
public:
    FixedInputChannelDatasetTableModel(const QString& node_uid,
                                       unsigned int dataset_type,
                                       QObject *parent = nullptr);
    void updateData(const QSharedPointer<chaos::common::data::CDataWrapper>& _dataset) Q_DECL_OVERRIDE;
    Qt::ItemFlags flags(const QModelIndex &index) const Q_DECL_OVERRIDE;
    Qt::DropActions supportedDragActions() const Q_DECL_OVERRIDE;
    QStringList mimeTypes() const Q_DECL_OVERRIDE;
    QMimeData *mimeData(const QModelIndexList &indexes) const Q_DECL_OVERRIDE;
    void updateInstanceDescription(const QSharedPointer<chaos::common::data::CDataWrapper>& _dataset_attribute_configuration);
    //! return all changed value and reset the tracking for new chagnes
    void getAttributeChangeSet(std::vector<ChaosSharedPtr<chaos::metadata_service_client::api_proxy::control_unit::ControlUnitInputDatasetChangeSet> > &value_set_array);
    void applyChangeSet(bool commit);
protected:
    int getRowCount() const Q_DECL_OVERRIDE;
    int getColumnCount() const Q_DECL_OVERRIDE;
    QString getHeaderForColumn(int column) const Q_DECL_OVERRIDE;
    QVariant getCellData(int row, int column) const Q_DECL_OVERRIDE;
    QVariant getTooltipTextForData(int row, int column) const Q_DECL_OVERRIDE;
    QVariant getTextAlignForData(int row, int column) const Q_DECL_OVERRIDE;
    QVariant getBackgroundForData(int row, int column) const Q_DECL_OVERRIDE;
    QVariant getTextColorForData(int row, int column) const Q_DECL_OVERRIDE;
    bool setCellData(const QModelIndex &index, const QVariant &value) Q_DECL_OVERRIDE;
    bool isCellEditable(const QModelIndex &index) const Q_DECL_OVERRIDE;
    bool isCellSelectable(const QModelIndex &index) const Q_DECL_OVERRIDE;
private:
    //memorize all the attributes that have had their values changed
    boost::dynamic_bitset<> attribute_value_changed;
    //contains the value for set
    QVector< QSharedPointer<AttributeValueChangeSet> > attribute_set_value;
    QMap<int, QSharedPointer<chaos::common::data::CDataWrapper> >dataset_attribute_configuration;
};

#endif // FIXEDINPUTCHANNELDATASETTABLEMODEL_H
