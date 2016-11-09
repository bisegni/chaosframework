#ifndef ABSTRACTBINARYPLOTADAPTER_H
#define ABSTRACTBINARYPLOTADAPTER_H

#include <chaos/common/chaos_constants.h>
#include <chaos/common/data/CDataBuffer.h>

#include <boost/shared_ptr.hpp>

class AbstractBinaryPlotAdapter {
public:
    AbstractBinaryPlotAdapter(boost::shared_ptr<chaos::common::data::CDataBuffer>& _buffer_to_plot,
                              std::vector<chaos::DataType::BinarySubtype>& _bin_type);
    ~AbstractBinaryPlotAdapter();

    const unsigned int getChannelNumber();
    void iterOnChannel(const unsigned int channel_index);
protected:
    /*
    void channelIteration(const unsigned int channel_index,
                          const unsigned int element_index,
                          chaos::DataType::BinarySubtype element_type) {
        switch(element_type) {
        case SUB_TYPE_BOOLEAN:
            break;
        case SUB_TYPE_CHAR:
            break;
        case SUB_TYPE_INT8:
            break;
        case SUB_TYPE_INT16:
            break;
        case SUB_TYPE_INT32:
            break;
        case SUB_TYPE_INT64:
            break;
        case SUB_TYPE_DOUBLE:
            break;
        }
    }
*/
    virtual void channelIteration(const unsigned int channel_index,
                                  const unsigned int element_index,
                                  chaos::DataType::BinarySubtype element_type,
                                  void *element_value) = 0;

private:
    unsigned int channel_number;
    boost::shared_ptr<chaos::common::data::CDataBuffer> buffer_to_plot;
    std::vector<chaos::DataType::BinarySubtype> bin_type;
};


template<typename T>
class SingleTypeBinaryPlotAdapter:
        public AbstractBinaryPlotAdapter {
    SingleTypeBinaryPlotAdapter(boost::shared_ptr<chaos::common::data::CDataBuffer>& _buffer_to_plot,
                                std::vector<chaos::DataType::BinarySubtype>& _bin_type):
        AbstractBinaryPlotAdapter(_buffer_to_plot,
                                  _bin_type){}

    ~SingleTypeBinaryPlotAdapter();

protected:

    void channelIteration(const unsigned int channel_index,
                          const unsigned int element_index,
                          chaos::DataType::BinarySubtype element_type,
                          void *element_value) {
        channelElement(channel_index,
                       element_index,
                       *((T*)element_value));
    }

    virtual void channelElement(const unsigned int channel_index,
                                const unsigned int element_index,
                                const T& element_value) = 0;
};
#endif // ABSTRACTBINARYPLOTADAPTER_H
