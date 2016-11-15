#include "AbstractBinaryPlotAdapter.h"

AbstractBinaryPlotAdapter::AbstractBinaryPlotAdapter():
    bin_type(0),
    channel_number(0),
    channel_offset(0),
    element_per_channel(0){}

AbstractBinaryPlotAdapter::~AbstractBinaryPlotAdapter() {}

void AbstractBinaryPlotAdapter::setBinaryType(std::vector<chaos::DataType::BinarySubtype>& _bin_type) {
    bin_type = _bin_type;
    channel_offset = 0;
    channel_number = _bin_type.size();
    for(unsigned int idx = 0; idx < channel_number; idx++) {channel_offset += getChannelSize(idx);}
    element_per_channel = (_bin_type.size()/channel_offset);
}

void AbstractBinaryPlotAdapter::setData(boost::shared_ptr<chaos::common::data::CDataBuffer>& _buffer_to_plot) {
    buffer_to_plot = _buffer_to_plot;
}

unsigned int AbstractBinaryPlotAdapter::getChannelNumber() {
    return channel_number;
}

unsigned int AbstractBinaryPlotAdapter::getNumberOfElementPerChannel() {
    return element_per_channel;
}

unsigned int AbstractBinaryPlotAdapter::getChannelSize(const unsigned int channel_index) {
    if(channel_index >= channel_number) return 0;
    switch(CHAOS_SUBTYPE_UNWRAP(bin_type[channel_index])) {
    case chaos::DataType::SUB_TYPE_BOOLEAN: return sizeof(bool); break;
    case chaos::DataType::SUB_TYPE_CHAR:return sizeof(char); break;
    case chaos::DataType::SUB_TYPE_INT8:return sizeof(int8_t); break;
    case chaos::DataType::SUB_TYPE_INT16:return sizeof(int16_t); break;
    case chaos::DataType::SUB_TYPE_INT32:return sizeof(int32_t); break;
    case chaos::DataType::SUB_TYPE_INT64:return sizeof(int64_t); break;
    case chaos::DataType::SUB_TYPE_DOUBLE:return sizeof(double); break;
    default: return 0; break;
    }
}

void AbstractBinaryPlotAdapter::iterOnChannel(const unsigned int channel_index) {
    //iter on single channel(skipping interleaved ones)
    if(channel_index >= channel_number) return;
    bool is_unsigned = CHAOS_SUBTYPE_IS_UNSIGNED(bin_type[channel_index]);
    chaos::DataType::BinarySubtype subtype = static_cast<chaos::DataType::BinarySubtype>(CHAOS_SUBTYPE_UNWRAP(bin_type[channel_index]));

    uint32_t element_position = 0;
    for(unsigned int idx = 1; idx < channel_index; idx++) {element_position += getChannelSize(idx);}

    for(unsigned int element = 0; element_position<buffer_to_plot->getBufferSize();) {
        channelIteration(channel_index,
                         element++,
                         is_unsigned,
                         subtype,
                         (void*)(buffer_to_plot->getBuffer()+element_position));
        element_position += channel_offset;
    }
}
