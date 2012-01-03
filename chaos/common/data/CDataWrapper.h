    //
    //  CDataWrapper.h
    //  ControlSystemLib
    //
    //  Created by Claudio Bisegni on 23/04/11.
    //  Copyright 2011 INFN. All rights reserved.
    //

#ifndef CDataWrapper_H
#define CDataWrapper_H
#include <boost/scoped_ptr.hpp>
#include <string>
#include <vector>
#include <chaos/common/bson/bson.h>
#include <chaos/common/bson/util/json.h>
#include <chaos/common/pool/CPoolMemoryObject.h>

namespace chaos {
    using namespace std;
    using namespace bson;
    using namespace boost;
    
    class CDataWrapper;
    /*
     Class to read the and arry of multivalue
     */
    class CMultiTypeDataArrayWrapper {
        vector< BSONElement > elementsArray;
        
    public:
        CMultiTypeDataArrayWrapper(vector< BSONElement > src);
        
        string getStringElementAtIndex(const int);
        double_t getDoubleElementAtIndex(const int);
        int32_t getInt32ElementAtIndex(const int);
        int64_t getint64ElementAtIndex(const int);
        CDataWrapper* getCDataWrapperElementAtIndex(const int);
        
        vector<BSONElement>::size_type size() const;
    };
    
    /*
     Class for contain the serialization buffer
     the class deallocation will dealloc all the 
     serialization buffer
     */
    class SerializationBuffer {
        size_t bSize;
        char *buffer;
    public:
        SerializationBuffer(const char *iBuff, size_t iSize){
            bSize = iSize;
            buffer = 0L;
            
            if(iBuff && iSize){
                buffer = new char[iSize];
                std::memcpy(buffer, iBuff, iSize);
            }
        }
        
        ~SerializationBuffer(){
            if(buffer)delete buffer;
        }
        size_t getBufferLen(){return bSize;};
        const char *getBufferPtr(){return buffer;};
    };
    
    /*
     Class that wrap the serializaiton system for data storage
     */
    class CDataWrapper/*: public CPoolMemoryObject<CDataWrapper> */{
        scoped_ptr<BSONObjBuilder> bsonBuilder;
        scoped_ptr<BSONArrayBuilder> bsonArrayBuilder;
    public:
        
        CDataWrapper();
        CDataWrapper(const char* serializationBuffer, bool bson=true);
            //add a csdata value
        void addCSDataValue(const char *, CDataWrapper&);
        
            //get a csdata value
        CDataWrapper *getCSDataValue(const char *);
        
            //add a string value
        void addStringValue(const char *, const char *);
        
            //add a string value
        void addStringValue(const char *, string&);

            //add a string to array
        void appendStringToArray(const char *);
        
            //add a strin gto array
        void appendStringToArray(string&);
        void appendInt32ToArray(int32_t int32ArrayElement);
        void appendInt64ToArray(int64_t int64ArrayElement);
        void appendDoubleToArray(double_t doubleArrayElement);
        void appendCDataWrapperToArray(CDataWrapper& srcDataWrapper, bool finalize=false);
            //finalize the array into a key for the current dataobject
        void finalizeArrayForKey(const char *);

            //get a string value
        string  getStringValue(const char *);
        
            //return a vectorvalue for a key
        CMultiTypeDataArrayWrapper* getVectorValue(const char *);
        
            //add a integer value
        void addInt32Value(const char *, int32_t);

#if __linux__ && !defined(__x86_64__)
        void addDoubleValue(const char *key, double dValue);
#endif
        void addDoubleValue(const char *key, double_t dValue);
        
            //add a integer value
        void addInt64Value(const char *, int64_t);
        
            //add a integer value
        int32_t getInt32Value(const char *key);
        
            //add a integer value
        int64_t getInt64Value(const char *key);
        
            //add a integer value
        double_t getDoubleValue(const char *key);
        
            //add a bool value
        void addBoolValue(const char *, bool);
        
            //get a bool value
        bool getBoolValue(const char *);
        
            //set a binary data value
        void addBinaryValue(const char *, const char *, int);
        
            //return the binary data value
        const char* getBinaryValue(const char *, int&);
        
            //return the bson data
        SerializationBuffer* getBSONData();
        
            //return the json data
        SerializationBuffer* getJSONData();
        
            //return the json representation for this data wrapper
        string getJSONString();
        
            //reinitialize the object with bson data
        void setSerializedData(const char* bsonData, bool bson=true);
        
            //check if the key is present in data wrapper
        bool hasKey(const char*);
        
            //reset the datawrapper
        void reset();
            
            //append all element of an data wrapper
        void appendAllElement(CDataWrapper&);
    };
}


#endif
