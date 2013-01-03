//
//  SimpleDataTransform.h
//  CachingSystem
//
//  Created by Flaminio Antonucci on 15/11/12.
//
//

#ifndef CachingSystem_SimpleDataTransform_h
#define CachingSystem_SimpleDataTransform_h
#include "chaos/common/caching_system/caching_thread/tracker_interface/DataTransformFilterInterface.h"
#include "MioElemento.h"
using namespace chaos;
class SimpleDataTransform : public caching_system::DataTransformFilterInterface<Magnete> {
    
    
public:
    
    SimpleDataTransform(){}
     void trasformData(chaos::caching_system::DataElement<Magnete>* data){
    
        
         Magnete* magnete= data->getData();
         double* alto=(magnete->getTop());
         double* bottom=(magnete->getDown());
         double* left=(magnete->getLeft());
         double* right=(magnete->getRight());
         
         *alto=(*alto)/2;
         *bottom=(*bottom)/2;
         *left=(*left)/2;
         *right=(*right)/2;

         
    }

};


#endif
