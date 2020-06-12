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
            typedef std::vector<chaos::common::data::CDWShrdPtr> msg_queue_t;

            class MessagePublishSubscribeBase {

                public:
                typedef void (*msgpshandler)(const msg_queue_t&data,const int error_code);
                typedef struct msgstats {
                    uint64_t counter;
                    uint64_t oks;
                    uint64_t errs;
                    int last_err;
                    msgstats():counter(0),oks(0),errs(0),last_err(0){}
                } msgstats_t;
                enum eventTypes{
                    ONDELIVERY,
                    ONARRIVE,
                    ONERROR
                };
                protected:
                std::set<std::string> servers;
                std::string id;
                std::map< eventTypes,msgpshandler> handlers;
                msgstats_t stats;
                MessagePublishSubscribeBase*impl;
                uint64_t    counter,oks,errs;

                public:
                MessagePublishSubscribeBase(const std::string& id){};

                msgstats_t getStats() const{ return stats;}

                virtual ~MessagePublishSubscribeBase(){};
                void addServer(const std::string&url);

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
                virtual int applyConfiguration();

                /**
                 * @brief Wait for the completion of a async operation
                 * 
                 * @param timeout timeout in ms
                 * @return int 0 if success, negative if error
                 */
                virtual int waitCompletion(const uint32_t timeout_ms=5000);


                /**
                 * @brief Delete a key
                 * 
                 * @param key key to remove
                 * @return int 0 on success
                 */
                virtual int deleteKey(const std::string& key);

                /**
                 * @brief Create a key
                 * 
                 * @param key key to add
                 * @return int 0 on success
                 */
                virtual int createKey(const std::string& key);

            };
        }
        }
        }
#endif