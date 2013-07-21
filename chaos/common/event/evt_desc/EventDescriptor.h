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
            //!event pack struct bit length
#define EVENT_DATA_BYTE_LENGTH         1024    //1024 byte * 8 bit
#define EVENT_DATA_BIT_LENGTH          8192    //1024 byte * 8 bit
        
            //!header
#define EVT_HEADER_BYTE_LENGTH          4       // in byte
#define EVT_HEADERS_SIGN_LENGTH         16      // in bit
#define EVT_HEADER_VER_LENGTH           3       // in bit
#define EVT_HEADER_LEN_LENGTH           13      // in bit
        
            //!type and priority
#define EVT_TYPE_AND_PRIORITY_BYTE_LENGTH       1   //in byte
#define EVT_TYPE_AND_PRIORITY_LENGTH            8   //in bit
#define EVT_TYPE_FIELD_LENGTH                   2   //in bit
#define EVT_PRIORITY_FIELD_LENGTH               6   //in bit
        
            //!data struct
#define EVT_DATA_MAX_BYTE_LENGTH        1019 //1021 byte length for data
#define EVT_DATA_MAX_BIT_LENGTH         8152 //1021 byte length for data
        
        
            //! offset for the code
#define EVT_SUB_CODE_OFFSET         5
            //! offset for the priority
#define EVT_SUB_CODE_PRIORITY_OFFSET      7
            //! offset for the data type

#define EVT_IDENTIFICATION_LENGTH_INFO_OFFSET 9
#define EVT_IDENTIFICATION_VALUE_INFO_OFFSET 10
        
        /*!
         \enum EventType
         This enum define the type of the events
         */
        typedef enum {
            EventTypeAlert = 0,     /**< The type associated to the alert event type */
            EventTypeInstrument,
            EventTypeCommand,
            EventTypeCustom
        } EventType;
        
        /*!
         \enum EventDataType
         A struct for define the type of the value of an event
         */
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
        struct EventHeader {
                //! signature for the Chaos Event protocoll 'CE'
            uint16_t    sign:EVT_HEADERS_SIGN_LENGTH;
                //! version of the protocoll
            uint16_t    version:EVT_HEADER_VER_LENGTH;
                //! length of the event pack
            uint16_t    length:EVT_HEADER_LEN_LENGTH;
        };
        
        /*!
         A struct for define length and the value for the sender identifier
         */
        struct EventTypeAndPriority {
                //! type of the event
            uint8_t    type:EVT_TYPE_FIELD_LENGTH;
                //! priority of the event
            uint8_t    priority:EVT_PRIORITY_FIELD_LENGTH;     
        };
        
            //forward declaration for event factory
        class EventFactory;
        
            //!Event base class
        /*!
         This is the base class for all other event type. His scope is to manage the comomn work for all event type
         */
        class EventDescriptor {
            friend class EventFactory;
            uint8_t     tmp8;
            uint32_t    tmp32;
        protected:
            EventType instanceType;
            uint8_t instancePriority;
                //! event data pointer
            unsigned char eventData[EVENT_DATA_BYTE_LENGTH];
            
            
                //!Set the code of the alert
            /*!
             Set the allert code for this event
             \param alertCode thecode of the alert
             */
            void setSubCode(uint16_t subCode);
            
            
                //!Set the priority of this alert
            /*!
             Set the allert priority
             \param alertPriority priority
             */
            void setSubCodePriority(uint16_t subCodePriority);
            
                //!Set the size of the pack
            /*
             This method is for internal use only to class tree, because this is used
             when the data of the pach isfilled by base and subclass
             \param newSize is the new size of the packet
             */
            void setEventDataLength(uint8_t newSize);
            
                //! Set the sender identificaiton information
            /*!
                Check if all data is well formed and fix the max size of the identifier to 255 byte. If data exeed it si truncked
             */
            inline uint16_t setSenderIdentification(const char * const identification = NULL, uint8_t identificationLength = 0);
            
            /*!
             Set the Value for the type and the identification, thia aslo adjust the total size of packet
             \param identification is the identification for the sender
             \param identificationLength is length of the indetification of the sender
             \param valueType the enumeration that descrive the type of the value with EventDataType constant
             \param valuePtr a pointer to the value
             \param valueSizethe size of the value
             */
            void setIdentificationAndValueWithType(const char * identification, uint8_t identificationLength, EventDataType valueType, const void *valuePtr, uint16_t valueSize = 0);
            
        public:
            
                //!Create a new event descriptor
            /*
             Initializze all information to manage an event type. Can also create
             a event constructor by already instatiated memory
             */
            EventDescriptor(EventType _instanceType, uint8_t _instancePriority);
            
                //!Destructor of the event descriptor
            /*
             If this instance own the memory it wil be deallocated
             */
            virtual ~EventDescriptor();
            
                //!Event data inizialization
            /*
             Initialize the data for the base descriptor competence. These are
             only the header and base length (header only length)
             */
            void initData();
            
                //!Return the dversion form te header
            /*
             Return the version of the protocol wrapped on pack
             */
            uint16_t getEventHeaderVersion();
            
                //!Return the data length of the event
            /*
             Return the data length of the event contextually to the event type
             */
            uint16_t getEventDataLength();
            
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
            
            const char * const getIdentification();
            
            uint8_t getIdentificationlength();
            
            
                //!Return the custom code of this alert
            /*!
             Return the alert custom code identified by this event
             \return the custom code of the alert
             */
            uint16_t getSubCode();
            
            
                //!Return the code of this alert
            /*!
             Return the alert code identified bythis event
             \return the code of the alert
             */
            uint16_t getSubCodePriority();
            
            EventDataType getEventValueType();
            
            uint16_t getEventValueSize();
            
            void getEventValue(void *valuePtr, uint16_t *size);

            
        };
    }
}


#endif /* defined(__CHAOSFramework__EventDescriptor__) */
