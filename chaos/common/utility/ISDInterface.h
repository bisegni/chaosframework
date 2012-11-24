//
//  ISDInterface.h
//  CHAOSFramework
//
//  Created by Claudio Bisegni on 26/08/12.
//  Copyright (c) 2012 INFN. All rights reserved.
//

#ifndef CHAOSFramework_ISDInterface_h
#define CHAOSFramework_ISDInterface_h

#include <chaos/common/data/CDataWrapper.h>
#include <chaos/common/exception/CException.h>

namespace chaos {
    namespace utility {
            //! comodity interface for init, deinit and start operation
        /*!
         all class that need initialization phases need to be exetends this class
         */
        class ISDInterface {
        public:
            
            typedef enum {
                ISD_INITING,
                ISD_INITIATED,
                ISD_DEINITING,
                ISD_DEINTIATED
            } ISDState;
            
            /*!
             */
            ISDInterface();
            
            /*!
             */
            ~ISDInterface();
            
                //! Initialize instance
            virtual void init(CDataWrapper*) throw(chaos::CException) = 0;
            
                //! Start the implementation
            virtual void start() throw(chaos::CException) = 0;
            
                //! Deinit the implementation
            virtual void deinit() throw(chaos::CException) = 0;

            static bool initImplementation(ISDInterface *impl, CDataWrapper *initData, const char * const implName,  const char * const domainString);
            static bool startImplementation(ISDInterface *impl, const char * const implName,  const char * const domainString);
            static bool deinitImplementation(ISDInterface *impl, const char * const implName,  const char * const domainString);
        protected:
            ISDState isdState;
        };
    }
}

#endif
