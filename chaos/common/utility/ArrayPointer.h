    //
    //  ArrayPointer.h
    //  ControlSystemLib
    //
    //  Created by Bisegni Claudio on 31/07/11.
    //  Copyright 2011 INFN. All rights reserved.
    //

#ifndef ChaosLib_ArrayPointer_h
#define ChaosLib_ArrayPointer_h

#include <vector>
#include <boost/ptr_container/ptr_vector.hpp>

namespace chaos {
    using namespace std;
    using namespace boost;
    /*
     This class represent a groupo or list of a CDataWrapper
     Class
     */
    template <typename T>
    class ArrayPointer{
        vector<T*> pointerArray;
    public:
        
        typedef typename vector<T*>::iterator ArrayPointerIterator;
        
        /*
         Destructor
         */
        ~ArrayPointer() {
            clear();
        }
        
        /*
         Add a new object pointer
         */
        void add(T *objPtr){
            pointerArray.push_back(objPtr); 
        }
        
        
        /*
         Clear all the array with contained pointer
         */
        void clear(){
                //we need to delete all remainig element
            for (typename vector<T*>::iterator tIter = pointerArray.begin(); 
                 tIter != pointerArray.end(); 
                 tIter++) {
                delete *tIter;
            }
        }
        
        /*
         return poitner with [] operatore
         */
        T* operator[](int i) { return pointerArray[i]; }
        
        /*
         return the number of objet pointer managed
         */
        typename vector<T*>::size_type size() const { return pointerArray.size(); };

        ArrayPointerIterator begin(){ return pointerArray.begin(); }
        
        ArrayPointerIterator end(){ return pointerArray.end(); }
    }; 
    
}  


#endif
