//
//  BsonFragment.cpp
//  CHAOSFramework
//
//  Created by Claudio Bisegni on 4/23/13.
//  Copyright (c) 2013 INFN. All rights reserved.
//

#include <chaos/common/data/BsonFragment.h>

using namespace chaos::data;

BsonFragment::BsonFragment():BSONObjBuilder(true, 512) {
    
}

const char *BsonFragment::getFragmentPtr(uint32_t& fragmetDimension) {
    const char *result = BSONObjBuilder::_b.buf();
    fragmetDimension = BSONObjBuilder::_b.len();
    BSONObjBuilder::_b.decouple();
    BSONObjBuilder::_b.reset();
    return result;
}