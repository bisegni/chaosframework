/*
 * Copyright 2012, 2017 INFN
 *
 * Licensed under the EUPL, Version 1.2 or – as soon they
 * will be approved by the European Commission - subsequent
 * versions of the EUPL (the "Licence");
 * You may not use this work except in compliance with the
 * Licence.
 * You may obtain a copy of the Licence at:
 *
 * https://joinup.ec.europa.eu/software/page/eupl
 *
 * Unless required by applicable law or agreed to in
 * writing, software distributed under the Licence is
 * distributed on an "AS IS" basis,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied.
 * See the Licence for the specific language governing
 * permissions and limitations under the Licence.
 */

#include <chaos/common/data/cache/AttributeCache.h>
#include <chaos/common/global.h>
#include <boost/format.hpp>

using namespace chaos::common::data::cache;
using namespace std;

#pragma mark AttributeCache

AttributeCache::AttributeCache():
		index(0),
		mutex(new boost::shared_mutex()){}

AttributeCache::~AttributeCache() {}

void AttributeCache::reset() {
	index = 0;
	mapAttributeNameIndex.clear();
	vector_attribute_value.clear();
	bitmapChangedAttribute.clear();
}

void AttributeCache::addAttribute(const string& name,
		uint32_t size,
		chaos::DataType::DataType type,
		const std::vector<chaos::DataType::BinarySubtype>& sub_type) {
	if(mapAttributeNameIndex.count(name)) return;

	VariableIndexType tmpIndex;

	//add name nad his index
	mapAttributeNameIndex.insert(make_pair(name, (tmpIndex=index++)));

	//add channel setting
	switch(type) {
	case chaos::DataType::TYPE_BOOLEAN:
		size = sizeof(bool);
		break;
	case chaos::DataType::TYPE_DOUBLE:
		size = sizeof(double);
		break;
	case chaos::DataType::TYPE_INT32:
		size = sizeof(int32_t);
		break;
	case chaos::DataType::TYPE_INT64:
		size = sizeof(int64_t);
		break;

	default:
		break;
	}
	ChaosSharedPtr<AttributeValue> tmpSP(new AttributeValue(name, tmpIndex, size, type, sub_type));

	//add the relative bit
	bitmapChangedAttribute.push_back(false);
	tmpSP->sharedBitmapChangedAttribute = &bitmapChangedAttribute;

	vector_attribute_value.push_back(tmpSP);
}

void AttributeCache::getAttributeNames(std::vector<std::string>& names) {

	//get all names
	for(map<string, VariableIndexType>::iterator it = mapAttributeNameIndex.begin();
			it != mapAttributeNameIndex.end();
			it++) {
		names.push_back(it->first);
	}
}

void AttributeCache::setValueForAttribute(VariableIndexType n,
		const void * value,
		uint32_t size) {
	CHAOS_ASSERT(n<vector_attribute_value.size());
	CHAOS_ASSERT(vector_attribute_value[n].get());
	vector_attribute_value[n]->setValue(value, size);
}

void AttributeCache::setValueForAttribute(VariableIndexType n,
		CDataWrapper& value) {
	CHAOS_ASSERT(n<vector_attribute_value.size());
	CHAOS_ASSERT(vector_attribute_value[n].get());
	vector_attribute_value[n]->setValue(value);
}

void AttributeCache::setValueForAttribute(const std::string& name,
		const void * value,
		uint32_t size) {
	if(mapAttributeNameIndex.count(name)==0){
		throw chaos::CFatalException(-1, boost::str(boost::format("No name '%1%' present in Attribute cache") %name), __PRETTY_FUNCTION__);

	}
	vector_attribute_value[mapAttributeNameIndex[name]]->setValue(value, size);
}

VariableIndexType AttributeCache::getIndexForName(const std::string& name ) {
	if(mapAttributeNameIndex.count(name)==0){
		throw chaos::CFatalException(-1, boost::str(boost::format("No name '%1%' present in Attribute cache") %name), __PRETTY_FUNCTION__);
	}
	return mapAttributeNameIndex[name];
}

bool AttributeCache::hasName(const std::string& name) {
	return mapAttributeNameIndex.count(name) != 0;
}

AttributeValue *AttributeCache::getValueSettingForIndex(VariableIndexType index) {
	if(index<vector_attribute_value.size())
		return vector_attribute_value[index].get();
	throw chaos::CFatalException(-1, boost::str(boost::format("Attribute index '%1%' out of bounds max '%2%'") %index %vector_attribute_value.size()), __PRETTY_FUNCTION__);
}

AttributeValue *AttributeCache::getValueSettingByName(const std::string& name) {
	if(mapAttributeNameIndex.count(name)==0){
		throw chaos::CFatalException(-1, boost::str(boost::format("No name '%1%' present in Attribute cache") %name), __PRETTY_FUNCTION__);
	}
	return vector_attribute_value[mapAttributeNameIndex[name]].get();
}

VariableIndexType AttributeCache::getNumberOfAttributes() {
	return index;
}

void AttributeCache::getChangedIndex(std::vector<VariableIndexType>& changed_index) {
	size_t index = 0;
	index = bitmapChangedAttribute.find_first();
	while(index != boost::dynamic_bitset<BitBlockDimension>::npos) {
		changed_index.push_back(index);
		index = bitmapChangedAttribute.find_next(index);
	}
}

void AttributeCache::resetChangedIndex() {
	bitmapChangedAttribute.reset();
}

void AttributeCache::markAllAsChanged() {
	bitmapChangedAttribute.set();
}

bool AttributeCache::hasChanged() const {
	return bitmapChangedAttribute.any();
}

bool AttributeCache::setNewSize(VariableIndexType attribute_index,
		uint32_t new_size,
		bool clear_mem) {
	if(attribute_index>=vector_attribute_value.size()) {
		throw chaos::CFatalException(-1, boost::str(boost::format("No %1% index present in Attribute cache") %index), __PRETTY_FUNCTION__);
	}
	return vector_attribute_value[attribute_index]->setNewSize(new_size,
			clear_mem);
}

bool AttributeCache::setNewSize(const std::string& attribute_name,
		uint32_t new_size,
		bool clear_mem) {
	if(!mapAttributeNameIndex.count(attribute_name)) {
		throw chaos::CFatalException(-1, boost::str(boost::format("No '%1%' name present in Attribute cache") %index), __PRETTY_FUNCTION__);
	}
	return vector_attribute_value[mapAttributeNameIndex[attribute_name]]->setNewSize(new_size,
			clear_mem);
}

bool AttributeCache::hasAttribute(const std::string& attribute_name) {
	return mapAttributeNameIndex.count(attribute_name) != 0;
}

void AttributeCache::writeAttributeToCDataWrapper(const std::string& attribute_name,
		CDataWrapper& dest_dw) {
	if(mapAttributeNameIndex.count(attribute_name) == 0) return;
	//we can fill the cdatawrapper with key value
	vector_attribute_value[mapAttributeNameIndex[attribute_name]]->writeToCDataWrapper(dest_dw);
}

void AttributeCache::exportToCDataWrapper(CDataWrapper& dest_dw) const {
	for(AttributeValueVectorConstIterator it = vector_attribute_value.begin(),
			end = vector_attribute_value.end();
			it != end;
			it++) {
		//add attribute to the cdata wrapper
		(*it)->writeToCDataWrapper(dest_dw);
	}
}

void AttributeCache::copyToAttributeCache(AttributeCache& dest_attribute_cache) {
	for(AttributeValueVectorConstIterator it = vector_attribute_value.begin(),
			end = vector_attribute_value.end();
			it != end;
			it++) {
		dest_attribute_cache.copyAttribute(*(*it));
	}
}

void AttributeCache::copyAttribute(AttributeValue& av) {
	if(hasAttribute(av.name)){
		AttributeValue*ptr=getValueSettingByName(av.name);
		LDBG_<<"copy attribute value '"<<av.name<<"' size:"<<av.size<<" type:"<<av.type<<" subtype:"<<((av.sub_type.size()>0)?av.sub_type[0]:-1)<<" IN size:"<<ptr->size<<" type:"<<ptr->type<<" subtype:"<<((ptr->sub_type.size()>0)?ptr->sub_type[0]:-1);

		//update attribute
		getValueSettingByName(av.name)->setValue(av.getAsVariant());
	} else {
//        DEBUG_CODE({
//            if(av.size && (av.type == 6) && av.value_buffer ){
//                LDBG_<<"adding attribute value '"<<av.name<<"' size:"<<av.size<<" type:"<<av.type<<" subtype:"<<((av.sub_type.size()>0)?av.sub_type[0]:-1)<<" stringa:"<<(const char*)av.value_buffer;
//            } else {
//                LDBG_<<"adding attribute value '"<<av.name<<"' size:"<<av.size<<" type:"<<av.type<<" subtype:"<<((av.sub_type.size()>0)?av.sub_type[0]:-1);
//            }
//        });
		//add attribute
		addAttribute(av.name,
				av.size,
				av.type,
				av.sub_type);
		//udpate value

		getValueSettingByName(av.name)->setValue(av.getAsVariant());
	}
}

#pragma mark SharedCacheLockDomain
SharedCacheLockDomain::SharedCacheLockDomain(ChaosSharedPtr<boost::shared_mutex>& _mutex):
		mutex(_mutex){}

SharedCacheLockDomain::~SharedCacheLockDomain(){}


#pragma mark WriteSharedCacheLockDomain

WriteSharedCacheLockDomain::WriteSharedCacheLockDomain(ChaosSharedPtr<boost::shared_mutex>& _mutex,
		bool lock):
		SharedCacheLockDomain(_mutex),
		w_lock(*mutex.get(), boost::defer_lock){if(lock)w_lock.lock();}

WriteSharedCacheLockDomain::~WriteSharedCacheLockDomain() {}

void WriteSharedCacheLockDomain::lock() {
	w_lock.lock();
}

void WriteSharedCacheLockDomain::unlock() {
	w_lock.unlock();
}


ReadSharedCacheLockDomain::ReadSharedCacheLockDomain(ChaosSharedPtr<boost::shared_mutex>& _mutex,
		bool lock):
		SharedCacheLockDomain(_mutex),
		r_lock(*mutex.get(), boost::defer_lock){if(lock)r_lock.lock();}

ReadSharedCacheLockDomain::~ReadSharedCacheLockDomain() {}

void ReadSharedCacheLockDomain::lock() {
	r_lock.lock();
}

void ReadSharedCacheLockDomain::unlock() {
	r_lock.unlock();
}
