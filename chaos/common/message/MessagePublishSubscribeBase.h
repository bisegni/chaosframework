/**
 * @brief Abstraction for message producer/subscribe
 * 
 */
#ifndef __MESSAGE_PUBLISHSUBSCRIBE_H__
#define __MESSAGE_PUBLISHSUBSCRIBE_H__
#include <set>
#include <string>
#include <chaos/common/data/CDataWrapper.h>
namespace chaos {
    namespace common {
        namespace message {

            class MessagePublishSubscribeBase {
                public:
                typedef int (*msgpshandler)(chaos::common::data::CDWUniquePtr&);
                enum eventTypes{
                    ONDELIVERY,
                    ONARRIVE,
                    ONERROR
                };
                protected:
                std::set<std::string> servers;
                std::string id;
                std::map< eventTypes,msgpshandler> handlers;
                public:
                MessagePublishSubscribeBase(const std::string& id):id(id){};
                virtual ~MessagePublishSubscribeBase(){};
                virtual void addServer(const std::string&url);

                /**
                 * @brief Add an handler to the message
                 * 
                 * @param ev 
                 */
                virtual int addHandler(eventTypes ev,msgpshandler);

                /**
                 * @brief library specific options
                 * 
                 * @param key option name
                 * @param value value
                 * @return int 
                 */
                virtual int setOption(const std::string&key,const std::string& value);

 /**
                 * @brief apply configuration 
                 * 
                 * @return 0 if success
                 */
                virtual int applyConfiguration()=0;





            };
        }
        }
        }
#endif