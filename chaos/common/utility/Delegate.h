//
//  ImpossibilityFastDelegate.h
//  CHAOSFramework
//
//  Created by Claudio Bisegni on 15/05/14.
//  Copyright (c) 2014 INFN. All rights reserved.
//

// copied from http://www.codeproject.com/Articles/11015/The-Impossibly-Fast-C-Delegates

#ifndef CHAOSFramework_ImpossibilityFastDelegate_h
#define CHAOSFramework_ImpossibilityFastDelegate_h

namespace chaos {
	namespace utility {
		namespace delegate {
			
			class Delegate {
			public:
				Delegate()
				: object_ptr(0)
				, stub_ptr(0)
				{}
				
				template <class T, void (T::*TMethod)()>
				static Delegate from_method(T* object_ptr)
				{
					Delegate d;
					d.object_ptr = object_ptr;
					d.stub_ptr = &method_stub<T, TMethod>; // #1
					return d;
				}
				
				void operator()() const
				{
					return (*stub_ptr)(object_ptr);
				}
				
			private:
				typedef void (*stub_type)(void* object_ptr);
				
				void* object_ptr;
				stub_type stub_ptr;
				
				template <class T, void (T::*TMethod)()>
				static void method_stub(void* object_ptr)
				{
					T* p = static_cast<T*>(object_ptr);
					return (p->*TMethod)(); // #2
				}
			};

		}
	}
}
#endif
