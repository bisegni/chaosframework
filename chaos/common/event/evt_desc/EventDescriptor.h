/*
 *	EventDescriptor.h
 *	!CHOAS
 *	Created by Bisegni Claudio.
 *
 *    	Copyright 2012 INFN, National Institute of Nuclear Physics
 *
 *    	Licensed under the Apache License, Version 2.0 (the "License");
 *    	you may not use this file except in compliance with the License.
 *    	You may obtain a copy of the License at
 *
 *    	http://www.apache.org/licenses/LICENSE-2.0
 *
 *    	Unless required by applicable law or agreed to in writing, software
 *    	distributed under the License is distributed on an "AS IS" BASIS,
 *    	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    	See the License for the specific language governing permissions and
 *    	limitations under the License.
 */

#ifndef __CHAOSFramework__EventDescriptor__
#define __CHAOSFramework__EventDescriptor__

#include <stdint.h>
#include <cstddef>
#include <chaos/common/exception/CException.h>
namespace chaos {
    namespace event {
            //event pack struct bit length
#define EVENT_DATA_BYTE_LENGTH              1024    //1024 byte * 8 bit
#define EVENT_DATA_BIT_LENGTH          8192    //1024 byte * 8 bit
        
            //header
#define EVT_HEADER_BYTE_LENGTH          4       // in byte
#define EVT_HEADERS_SIGN_LENGTH         16      // in bit
#define EVT_HEADER_VER_LENGTH           3       // in bit
#define EVT_HEADER_LEN_LENGTH           13      // in bit
        
            //type and priority
#define EVT_TYPE_AND_PRIORITY_BYTE_LENGTH       1   //in byte
#define EVT_TYPE_AND_PRIORITY_LENGTH            8   //in bit
#define EVT_TYPE_FIELD_LENGTH                   2   //in bit
#define EVT_PRIORITY_FIELD_LENGTH               6   //in bit
        
            //data struct
#define EVT_DATA_MAX_BYTE_LENGTH        1019 //1021 byte length for data
#define EVT_DATA_MAX_BIT_LENGTH         8152 //1021 byte length for data
        
        typedef enum {
            EventDataInt8 = 0,
            EventDataInt16,
            EventDataInt32,
            EventDataInt64,
            EventDataDouble,
            EventDataCString,
            EventDataBinary
        } EventDataType;
        
        /*!
         A struct for define the header of a chaos event pack
         */
        typedef struct {
            /*@{*/
            uint16_t    sign:EVT_HEADERS_SIGN_LENGTH;   /**< signature for the Chaos Event protocoll 'CE' */
            uint16_t    version:EVT_HEADER_VER_LENGTH;  /**< version of the protocoll */
            uint16_t    length:EVT_HEADER_LEN_LENGTH;   /**< length of the event pack */
            /*@}*/
        } EventHeader;
        
        /*!
         A struct for define type and the priority of the event
         */
        typedef struct {
            /*@{*/
            uint8_t    type:EVT_TYPE_FIELD_LENGTH;             /**< typeoftheevent */
            uint8_t    priority:EVT_PRIORITY_FIELD_LENGTH;     /**< priority ofthe event */
            /*@}*/
        } EventTypeAndPriority;
        
        
        class EventFactory;
        
            //!Event base class
        /*
         This is the base class for all other event type. His scope is to manage the comomn work for all event type
         */
        class EventDescriptor {
            friend class EventFactory;
        protected:
                //! event data pointer
            unsigned char * eventData;
            
                //!Set the size of the pack
            /*
             This method is for internal use only to class tree, because this is used
             when the data of the pach isfilled by base and subclass
             \param newSize is the new size of the packet
             */
            void setEventDataLength(uint8_t newSize);
            
        public:
            
                //!Create a new event descriptor
            /*
             Initializze all information to manage an event type. Can also create
             a event constructor by already instatiated memory
             */
            EventDescriptor();
            
                //!Destructor of the event descriptor
            /*
             If this instance own the memory it wil be deallocated
             */
            ~EventDescriptor();
            
                //!Event data inizialization
            /*
             Initialize the data for the base descriptor competence. These are
             only the header and base length (header only length)
             */
            void initData();
            
                //!Event data inizialization with external memory
            /*
             This static method can be used to create an event indetified by external memory. memory can be an array as in the following example: char data[1024];
             Check are done to memory dimension
             \tparam E classthat identify the event type:
             - AlertEventDescriptor
             - InstrumentEventDescriptor
             - CommandEventDescriptor
             - CustomEventDescriptor
             */
            template <typename E, typename T, int N>
            static EventDescriptor *initDataWithMemory(const unsigned char * const existingEventData) throw (CException) {
                if(N != EVENT_DATA_BYTE_LENGTH)
                    throw CException(0, "Event memory need to be 1024 byte length", "EventDescriptor::initDataWithMemory");
                    
                        //check the memory regior for the type to get wich class need to be extended
                    
                    }
            
                //!Return the dversion form te header
            /*
             Return the version of the protocol wrapped on pack
             */
            uint8_t getEventHeaderVersion();
            
                //!Return the data length of the event
            /*
             Return the data length of the event contextually to the event type
             */
            uint8_t getEventDataLength();
            
                //!Return the data  of the event
            /*
             Return the data of the event
             */
            const unsigned char * const getEventData();
            
            
                //!Create a new event from serialized data
            /*
             Initializze all information to manage an event type. Can also create
             a event constructor by already instatiated memory
             \param serializedEvent pointer to an event data, the buff this is length 1024 byte
             \param length
             */
            void setEventData(const unsigned char *serializedEvent,  uint16_t length);

            
                //!Return the type  of the event
            /*
             Return the data of the event
             */
            uint8_t getEventType();
            
                //!Return the priority  of the event
            /*
             Return the data of the event
             */
            uint8_t getEventPriority();
        };
    }
}


#endif /* defined(__CHAOSFramework__EventDescriptor__) */
