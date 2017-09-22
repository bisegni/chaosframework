#ifndef METATYPES_H
#define METATYPES_H

#include <string>
#include <stdint.h>
#include <QSharedPointer>
#include <chaos_metadata_service_client/ChaosMetadataServiceClient.h>
#include "data/ChaosByteArray.h"
#include "data/CommandReader.h"
#include "data/CommandParameterReader.h"
#include "data/DatasetReader.h"
#include "data/DatasetAttributeReader.h"
#include "data/delegate/TwoLineInformationItem.h"
#include <chaos/common/data/CDataWrapper.h>
#include <chaos/common/data/CDataVariant.h>

Q_DECLARE_METATYPE(std::string)
Q_DECLARE_METATYPE(int32_t)
Q_DECLARE_METATYPE(int64_t)
Q_DECLARE_METATYPE(uint64_t)
Q_DECLARE_METATYPE(QSharedPointer<ChaosByteArray>)
Q_DECLARE_METATYPE(QSharedPointer<CommandReader>)
Q_DECLARE_METATYPE(QSharedPointer<CommandParameterReader>)
Q_DECLARE_METATYPE(QSharedPointer<DatasetReader>)
Q_DECLARE_METATYPE(QSharedPointer<DatasetAttributeReader>)
Q_DECLARE_METATYPE(QSharedPointer<chaos::CException>)
Q_DECLARE_METATYPE(QSharedPointer<chaos::common::data::CDataWrapper>)
Q_DECLARE_METATYPE(QSharedPointer<TwoLineInformationItem>)
Q_DECLARE_METATYPE(QSharedPointer<chaos::metadata_service_client::api_proxy::node::CommandTemplate>)
Q_DECLARE_METATYPE(ChaosSharedPtr<chaos::metadata_service_client::api_proxy::node::CommandTemplate>)
Q_DECLARE_METATYPE(ChaosSharedPtr<chaos::common::data::SerializationBuffer>)
Q_DECLARE_METATYPE(ChaosSharedPtr<chaos::common::data::CDataWrapper>)
Q_DECLARE_METATYPE(ChaosSharedPtr<chaos::CException>)
Q_DECLARE_METATYPE(chaos::metadata_service_client::node_monitor::OnlineState)
Q_DECLARE_METATYPE(chaos::service_common::data::node::NodeInstance)
Q_DECLARE_METATYPE(chaos::common::data::CDataVariant)
Q_DECLARE_METATYPE(chaos::service_common::data::script::ScriptInstance)

#endif // METATYPES_H
