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
				static std::map<std::string,boost::shared_ptr<T> > instances;
				static boost::mutex mutex;
				
			protected:
				
				virtual int callInit(std::string st)=0;
				
			public:
				
				SingletonOnArguments(){}
				
				std::string getParams(){return init_params;}
				
				void setParams(std::string p){init_params=p;}
				
				static boost::shared_ptr<T>& getInstance(std::string initParams){
					boost::mutex::scoped_lock l(mutex);
					
					typename std::map<std::string, boost::shared_ptr<T> >::iterator i = instances.find(initParams);
					if(i!=instances.end()){
						LDBG_<<" for parameters "<<initParams<<" RETRIVING instance x"<<hex<<i->second;
						
						return (i->second);
					}
					T* tmp = new T();
					
					boost::shared_ptr<T> ret = boost::shared_ptr<T>(tmp);
					instances.insert(std::pair<string,boost::shared_ptr<T> >(initParams,ret));
					
					dynamic_cast<SingletonOnArguments* >(tmp)->setParams(initParams);
					LDBG_<<" for parameters "<<initParams<<" CREATING instance x"<<hex<<ret;
					
					return instances[initParams];
				}
				
				static void removeInstance( boost::shared_ptr<T>&t ){
					boost::mutex::scoped_lock l(mutex);
					
					typename std::map<std::string, boost::shared_ptr<T> >::iterator i = instances.begin();
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
			template <class T> std::map<std::string,boost::shared_ptr<T>  > SingletonOnArguments<T>::instances;
			template <class T> boost::mutex SingletonOnArguments<T>::mutex;
		}
	}
}
#endif /* defined(__CHAOSFramework__SingletonOnArguments__) */
