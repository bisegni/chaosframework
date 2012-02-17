/*	
 *	CObjectProcessingQueueListener.h
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
#ifndef CObjectProcessingQueueListener_H
#define CObjectProcessingQueueListener_H
/*
 Abstract class that implement some method for listen the execution on OBuffer element
 */
namespace chaos{
    template<class T>
    class CObjectProcessingQueueListener {
        
    public:
        /*
         Called befor the buffer will process the element, if this function
         return false, the process of this element wil be not executed, discardi
         the object
         */
        virtual bool elementWillBeProcessed(int bufferTag, T*)=0;
        virtual bool elementWillBeDiscarded(int bufferTag, T*)=0;
    };
}
#endif
