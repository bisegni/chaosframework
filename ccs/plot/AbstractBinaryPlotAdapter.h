#ifndef ABSTRACTBINARYPLOTADAPTER_H
#define ABSTRACTBINARYPLOTADAPTER_H

#include <chaos/common/chaos_constants.h>
#include <chaos/common/data/CDataBuffer.h>

#include <boost/shared_ptr.hpp>

class AbstractBinaryPlotAdapter {
public:
    AbstractBinaryPlotAdapter();
    ~AbstractBinaryPlotAdapter();
    void setBinaryType(const std::vector<unsigned int> &_bin_type);
    void setData(const boost::shared_ptr<chaos::common::data::CDataBuffer> &_buffer_to_plot);
    unsigned int getChannelNumber();
    unsigned int getNumberOfElementPerChannel();
    void iterOnChannel(const unsigned int channel_index);
    inline unsigned int getChannelSize(const unsigned int channel_index);
protected:

    virtual void channelIteration(const unsigned int channel_index,
                                  const unsigned int element_index,
                                  const bool is_unsigned,
                                  chaos::DataType::BinarySubtype element_type,
                                  void *element_value) = 0;

private:
    unsigned int channel_number;
    unsigned int channel_offset;
    unsigned int element_per_channel;
    boost::shared_ptr<chaos::common::data::CDataBuffer> buffer_to_plot;
    std::vector<unsigned int> bin_type;
};


template<typename T>
class SingleTypeBinaryPlotAdapter:
        public AbstractBinaryPlotAdapter {
public:
    SingleTypeBinaryPlotAdapter():
        AbstractBinaryPlotAdapter(){}

    ~SingleTypeBinaryPlotAdapter(){}

    void channelIteration(const unsigned int channel_index,
                          const unsigned int element_index,
                          const bool is_unsigned,
                          chaos::DataType::BinarySubtype element_type,
                          void *element_value) {
        switch(element_type) {
        case chaos::DataType::SUB_TYPE_BOOLEAN:
        case chaos::DataType::SUB_TYPE_CHAR:
        case chaos::DataType::SUB_TYPE_INT8:
        case chaos::DataType::SUB_TYPE_INT16:
        case chaos::DataType::SUB_TYPE_INT32:
        case chaos::DataType::SUB_TYPE_INT64:
        case chaos::DataType::SUB_TYPE_DOUBLE:
            channelElement(channel_index,
                           element_index,
                           *static_cast<T*>(element_value));
            break;
        default: break;
        }
    }
protected:
    virtual void channelElement(const unsigned int channel_index,
                                const unsigned int element_index,
                                const T& element_value) = 0;
};
#endif // ABSTRACTBINARYPLOTADAPTER_H
