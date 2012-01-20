//
//  ConfigurationManager.cpp
//  ChaosFramework
//
//  Created by Claudio Bisegni on 13/03/11.
//  Copyright 2011 INFN. All rights reserved.
//
#include "../../common/global.h"
#include "ConfigurationManager.h"

namespace chaos{
    using namespace std;
#pragma mark Private Method
    /*
     * Constructor
     */
    ConfigurationManager::ConfigurationManager(){
    
    }
    
    /*
     * Distructor
     */
    ConfigurationManager::~ConfigurationManager(){
            //clear all default domain configuration value
        clearDefaultDomainConfiguration();
    }
    
#pragma mark Public Method
    /*
     method for set default domain configuration
     */
    void ConfigurationManager::addDefaultDomainConfiguration(const char* key, const char* value) {
        defaultDomainConfiguration.insert(pair< string, string >(key,value));
    }
    
    /*
     method for set default domain configuration
     */
    void ConfigurationManager::addDefaultDomainConfiguration(const char* key, const char* value, long valueLen) {
        string valueToPush(value, valueLen);
        defaultDomainConfiguration.insert(pair< string, string >(key, valueToPush));
    }
    
    /*
     method for set default domain configuration
     */
    void ConfigurationManager::addDefaultDomainConfiguration(const char* key, string& value) {
        defaultDomainConfiguration.insert(pair< string, string >(key, value));
    }
    
    /*
     method for get default domain configuration values for a key
     */
    void ConfigurationManager::getDefaultDomainConfiguration(const char* key, vector< string >& valuesFound) {
            //fill input vector with all values for passed key
        multimap<string, string>::iterator iter = defaultDomainConfiguration.find(key);
        for ( ; iter != defaultDomainConfiguration.end(); iter++ ){
            valuesFound.push_back(iter->second);
        }

    }
    
    /*
     method for get default domain configuration values for a key
     */
    void ConfigurationManager::getDefaultDomainConfiguration(const char* key, string& valuesFound) {
            //fill input vector with all values for passed key
        multimap<string, string>::iterator iter = defaultDomainConfiguration.find(key);
        for ( ; iter != defaultDomainConfiguration.end(); iter++ ){
            valuesFound.assign(iter->second);
            break;
        }
        
    }
    
    /*
     method for get default domain configuration as CDataWrapper, this work if the
     valu ein the key is in json serialization
     */
    CDataWrapper *ConfigurationManager::getDefaultDomainConfigurationAsDataWrapper(const char* key) {
        CDataWrapper *result = NULL;
        multimap<string, string>::iterator iter = defaultDomainConfiguration.find(key);
        for ( ; iter != defaultDomainConfiguration.end(); iter++ ){
            result = new CDataWrapper(iter->second.c_str());
            break;
        }
        return result;
    }
    
    /*
     Remove all configuraiton values for default domain
     */
    void ConfigurationManager::clearDefaultDomainConfiguration() {
        defaultDomainConfiguration.clear();
    }
}