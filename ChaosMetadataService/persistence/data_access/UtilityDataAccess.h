/*
 * Copyright 2012, 2017 INFN
 *
 * Licensed under the EUPL, Version 1.2 or – as soon they
 * will be approved by the European Commission - subsequent
 * versions of the EUPL (the "Licence");
 * You may not use this work except in compliance with the
 * Licence.
 * You may obtain a copy of the Licence at:
 *
 * https://joinup.ec.europa.eu/software/page/eupl
 *
 * Unless required by applicable law or agreed to in
 * writing, software distributed under the Licence is
 * distributed on an "AS IS" basis,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied.
 * See the Licence for the specific language governing
 * permissions and limitations under the Licence.
 */

#ifndef __CHAOSFramework__UtilityDataAccess__
#define __CHAOSFramework__UtilityDataAccess__

#include "../persistence.h"

#include <chaos/common/data/CDataWrapper.h>

namespace chaos {
    namespace metadata_service {
        namespace persistence {
            namespace data_access {
                
                class UtilityDataAccess:
                public chaos::service_common::persistence::data_access::AbstractDataAccess{
                public:
                    DECLARE_DA_NAME

                    //! default constructor
                    UtilityDataAccess();
                    
                    //!default destructor
                    ~UtilityDataAccess();

                    //! return the next value for a named sequece
                    /*!
                     The sequced are named by a key, when thi api is called and the sequence not exists, a new entry is create
                     and initilized to first value that is euql to 1, and it is returned.
                     */
                    virtual int getNextSequenceValue(const std::string& sequence_name,
                                                     uint64_t& next_value) = 0;
                    
                    //! Create or update a named variable with a value
                    /*!
                     A variabl eis a global accessible variable that can be used anywhere and for general purpose,
                     it content is a buffer and is independent to it's content types
                     \param variable_name the name of the variable
                     \param cdw  object that store variable values
                     */
                    virtual int setVariable(const std::string& variable_name,
                                            chaos::common::data::CDataWrapper& cdw) = 0;
                    
                    //! Return, if has been set, the value of a variable
                    /*!
                     return the value stored into a variable,
                     if it has not been set, the buffer will be NULL 
                     with 0 size reported.
                     \param variable_name the name of the variable
                     \param cdw  object that store variable values
                     */
                    virtual int getVariable(const std::string& variable_name,
                                            chaos::common::data::CDataWrapper **cdw) = 0;
                    
                    //! delete the variable
                    /*!
                     \param variable_name the name of the variable
                     */
                    virtual int deleteVariable(const std::string& variable_name) = 0;

                    
                    //!reset al metadata service backend data
                    /*!
                     the reset is done without check (for now)
                     */
                    virtual int resetAllData() = 0;
                };
                
            }
        }
    }
}

#endif /* defined(__CHAOSFramework__UtilityDataAccess__) */
