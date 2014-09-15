//
//  SingletonOnArguments.h
//  CHAOSFramework
//
//  Created by andrea michelotti on 05/09/14.
//  Copyright (c) 2014 INFN. All rights reserved.
//

#ifndef __CHAOSFramework__SingletonOnArguments__
#define __CHAOSFramework__SingletonOnArguments__
#include <map>
#include <iostream>
namespace chaos{
    template <class T>
                class SingletonOnArguments {
                     std::string init_params;
                    static std::map<std::string,SingletonOnArguments*> instances;
                    
                protected:

                    virtual int callInit(std::string st)=0;

                public:

                    SingletonOnArguments(){}
                    
                    std::string getParams(){return init_params;}
                    
                    void setParams(std::string p){init_params=p;}
                    
                    static T* getInstance(std::string initParams){
                        typename std::map<std::string, SingletonOnArguments*>::iterator i = instances.find(initParams);
                        if(i!=instances.end()){
                            return dynamic_cast<T*>(i->second);
                        }
                        
                        T* ret = new T();
                        instances[initParams] = ret;
                        dynamic_cast<SingletonOnArguments*>(ret)->setParams(initParams);
                        return ret;
                    }

                    static void removeInstance(SingletonOnArguments*t ){
                        typename std::map<std::string,SingletonOnArguments*>::iterator i = instances.begin();
                        while(i!=instances.end()){
                            if(i->second==t){
                                instances.erase(i);
                            }
                            i++;
                        }
                    }

                    void removeInstance(void){
                        SingletonOnArguments<T>::removeInstance(this);
                    }
                    

                     int initFromParams(void){
                        return callInit(init_params);
                    }

                };
    template <class T> std::map<std::string,SingletonOnArguments<T>*> SingletonOnArguments<T>::instances;
}
#endif /* defined(__CHAOSFramework__SingletonOnArguments__) */
