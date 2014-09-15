//
//  SingletonOnArguments.cpp
//  CHAOSFramework
//
//  Created by andrea michelotti on 05/09/14.
//  Copyright (c) 2014 INFN. All rights reserved.
//

#include "SingletonOnArguments.h"

using namespace chaos;



template <class T>
std::map<std::string,T*> SingletonOnArguments<T>::instances;

template <class T>
T* SingletonOnArguments<T>::getInstance(std::string initParams){
    typename std::map<std::string, T*>::iterator i = instances.find(initParams);
    if(i!=instances.end()){
        return i->second;
    }
    
    T* ret = new T(initParams);
    instances[initParams] = ret;
    init_params = initParams;
    return ret;
}
template <class T>
void SingletonOnArguments<T>::removeInstance(T*t){
    typename std::map<std::string, T*>::iterator i = instances.begin();
    while(i!=instances.end()){
        if(i->second==t){
            instances.erase(i);
        }
        i++;
    }
}
template <class T>
void SingletonOnArguments<T>::removeInstance(){
    SingletonOnArguments<T>::removeInstance(this);
}

template <class T>
int SingletonOnArguments<T>::initFromParams(){
    return initFromParams(initParams];
}

