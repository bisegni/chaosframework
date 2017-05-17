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
#include <boost/utility.hpp>
#include <boost/thread/mutex.hpp>
#include <chaos/common/global.h>
#include <boost/shared_ptr.hpp>
namespace chaos{
	namespace common{
		namespace utility {
			template <class T>
			class SingletonOnArguments {
				std::string init_params;
				static std::map<std::string,ChaosSharedPtr<T> > instances;
				static boost::mutex mutex;
				
			protected:
				
				virtual int callInit(std::string st)=0;
				
			public:
				
				SingletonOnArguments(){}
				
				std::string getParams(){return init_params;}
				
				void setParams(std::string p){init_params=p;}
				
				static ChaosSharedPtr<T>& getInstance(std::string initParams){
					boost::mutex::scoped_lock l(mutex);
					
					typename std::map<std::string, ChaosSharedPtr<T> >::iterator i = instances.find(initParams);
					if(i!=instances.end()){
						return (i->second);
					}
					T* tmp = new T();
					
					ChaosSharedPtr<T> ret = ChaosSharedPtr<T>(tmp);
					instances.insert(std::pair<std::string,ChaosSharedPtr<T> >(initParams,ret));
					
					dynamic_cast<SingletonOnArguments* >(tmp)->setParams(initParams);
					
					return instances[initParams];
				}
				
				static void removeInstance( ChaosSharedPtr<T>&t ){
					boost::mutex::scoped_lock l(mutex);
					
					typename std::map<std::string, ChaosSharedPtr<T> >::iterator i = instances.begin();
					while(i!=instances.end()){
						if(i->second==t){
							instances.erase(i);
						}
						i++;
					}
				}
				
				
				
				int initFromParams(void){
					return callInit(init_params);
				}
				
			};
			template <class T> std::map<std::string,ChaosSharedPtr<T>  > SingletonOnArguments<T>::instances;
			template <class T> boost::mutex SingletonOnArguments<T>::mutex;
		}
	}
}
#endif /* defined(__CHAOSFramework__SingletonOnArguments__) */
