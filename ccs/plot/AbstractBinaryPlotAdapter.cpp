#include "AbstractBinaryPlotAdapter.h"

AbstractBinaryPlotAdapter::AbstractBinaryPlotAdapter(boost::shared_ptr<chaos::common::data::CDataBuffer>& _buffer_to_plot,
                                                     std::vector<chaos::DataType::BinarySubtype>& _bin_type):
    buffer_to_plot(_buffer_to_plot),
    bin_type(_bin_type),
    channel_number(_bin_type.size()){}

AbstractBinaryPlotAdapter::~AbstractBinaryPlotAdapter() {

}

const unsigned int AbstractBinaryPlotAdapter::getChannelNumber() {
    return channel_number;
}

void AbstractBinaryPlotAdapter::iterOnChannel(const unsigned int channel_index) {
    //iter on single channel(skipping interleaved ones)
}
