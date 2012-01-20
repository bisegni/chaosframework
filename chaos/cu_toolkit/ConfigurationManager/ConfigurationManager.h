    //
    //  ConfigurationManager.h
    //  ChaosFramework
    //
    //  Created by Claudio Bisegni on 13/03/11.
    //  Copyright 2011 INFN. All rights reserved.
    //
#ifndef ConfigurationManager_H
#define ConfigurationManager_H
#include <map>
#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>
#include "chaos/common/utility/Singleton.h"
#include "chaos/common/data/CDataWrapper.h"
namespace chaos{
    using namespace std;
    using namespace boost;
    /*
     * This class manage the configuraiton managing and creation
     * for all library
     */
        //DEFINE_CLASS_AS_SINGLETON(ConfigurationManager)
    class ConfigurationManager{
        //mutex for lock operation on service request
    boost::mutex managing_configuration_mutex;
    multimap<string, string> defaultDomainConfiguration;
    
    /*
     * Constructor
     */
    ConfigurationManager();
    
    /*
     * Distructor
     */
    ~ConfigurationManager();
public:
    /*
     method for set default domain configuration
     */
    void addDefaultDomainConfiguration(const char*, const char*);
    
    /*
     method for set default domain configuration with value and length of the value
     */
    void addDefaultDomainConfiguration(const char*, const char*, long);
    
    /*
     method for set default domain configuration with value and length of the value
     */
    void addDefaultDomainConfiguration(const char*, string&);
    
    /*
     method for get default domain configuration multi values for a key
     */
    void getDefaultDomainConfiguration(const char*, vector< string >&);
    
    /*
     method for get default domain configuration as CDataWrapper, this work if the
     valu ein the key is in json serialization
     */  
    CDataWrapper* getDefaultDomainConfigurationAsDataWrapper(const char*);
    
    /*
     method for get default domain configuration single values for a key
     */
    void getDefaultDomainConfiguration(const char*, string&);
    
    /*
     remove all default domain configuration
     */
    void clearDefaultDomainConfiguration();
    
    /*
     Configure the sandbox and all subtree of the CU
     */
    CDataWrapper* updateConfiguration(CDataWrapper*) {
        return NULL;
    }
};

}
#endif