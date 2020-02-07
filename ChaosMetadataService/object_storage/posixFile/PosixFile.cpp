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

#include "PosixFile.h"
#include <boost/filesystem.hpp>
#include <boost/algorithm/string/join.hpp>

#define INFO INFO_LOG(PosixFile)
#define DBG DBG_LOG(PosixFile)
#define ERR ERR_LOG(PosixFile)
#include <chaos/common/utility/TimingUtil.h>
using namespace chaos::metadata_service::object_storage;
using namespace boost::filesystem;
#define MAX_PATH_LEN 512
namespace chaos {
namespace metadata_service {
namespace object_storage {

PosixFile::PosixFile(const std::string& name)
    : basedatapath(name) {}

PosixFile::~PosixFile() {}

void PosixFile::calcFileDir(const std::string& prefix, const std::string& cu,const std::string& tag, uint64_t ts_ms, uint64_t seq, uint64_t runid, char* dir, char* fname) {
  // std::size_t found = cu.find_last_of("/");
  time_t     t     = (ts_ms / 1000);
  struct tm* tinfo = localtime(&t);
  // CU PATH NAME/<yyyy>/<mm>/<dd>/<hour>
  if(tag.size()>0){
    snprintf(dir, MAX_PATH_LEN, "%s/%s/%s", prefix.c_str(), cu.c_str(), tag.c_str());

  } else {
    snprintf(dir, MAX_PATH_LEN, "%s/%s/%.4d/%.2d/%.2d/%.2d", prefix.c_str(), cu.c_str(), tinfo->tm_year + 1900, tinfo->tm_mon+1, tinfo->tm_mday, tinfo->tm_hour);
  }
  // timestamp_runid_seq_ssss
  snprintf(fname, MAX_PATH_LEN, "%llu_%llu_%.10llu", t, runid, seq);
}

int PosixFile::pushObject(const std::string&                       key,
                          const ChaosStringSetConstSPtr            meta_tags,
                          const chaos::common::data::CDataWrapper& stored_object) {
  if (!stored_object.hasKey(chaos::DataPackCommonKey::DPCK_DEVICE_ID) ||
      !stored_object.hasKey(chaos::ControlUnitDatapackCommonKey::RUN_ID) ||
      !stored_object.hasKey(chaos::DataPackCommonKey::DPCK_SEQ_ID)) {
    ERR << CHAOS_FORMAT("Object to store doesn't has the default key!\n %1%", % stored_object.getJSONString());
    return -1;
  }
  uint64_t ts = chaos::common::utility::TimingUtil::getTimeStamp();
  char     dir[MAX_PATH_LEN];
  char     f[MAX_PATH_LEN];
  int64_t seq,runid;
  std::string tag;
  if(meta_tags->size()>0){
    //tag=std::accumulate(meta_tags->begin(),meta_tags->end(),std::string("_"));
    tag = boost::algorithm::join(*meta_tags.get(),"_");

  }
  seq=stored_object.getInt64Value(chaos::DataPackCommonKey::DPCK_SEQ_ID);
  runid=stored_object.getInt64Value(chaos::ControlUnitDatapackCommonKey::RUN_ID);
  calcFileDir(basedatapath, key,tag, ts,seq , runid, dir, f);
  if ((boost::filesystem::exists(dir) == false) &&
      (boost::filesystem::create_directories(dir) == false)) {
    ERR << "cannot create directory:" << dir;
   // throw chaos::CException(-1, __PRETTY_FUNCTION__, "cannot create directory:" + std::string(dir));
   return -1;
  }

  char fname[MAX_PATH_LEN*2];
  snprintf(fname,sizeof(fname),"%s/%s",dir,f);

  ofstream fil(fname, std::ofstream::binary);
  //DBG << "["<<ts<<"] WRITE \"" << dir << "\" seq:" << seq << " runid:" << runid << " data size:" << stored_object.getBSONRawSize();
  if(fil.is_open()){
    fil.write((const char*)stored_object.getBSONRawData(), stored_object.getBSONRawSize());
    fil.close();
  } else {
      ERR<<" CANNOT WRITE:"<<fname;
      return -2;
      }
  return 0;
}

//!Retrieve an object from the object persistence layer
int PosixFile::getObject(const std::string&               key,
                         const uint64_t&                  timestamp,
                         chaos::common::data::CDWShrdPtr& object_ptr_ref) {
  ERR << " NOT IMPLEMENTED";
  return -1;
}

//!Retrieve the last inserted object from the object persistence layer
int PosixFile::getLastObject(const std::string&               key,
                             chaos::common::data::CDWShrdPtr& object_ptr_ref) {
                                   ERR << " NOT IMPLEMENTED";

                                 return -1;
}

//!delete objects that are contained between intervall (exstreme included)
int PosixFile::deleteObject(const std::string& key,
                            uint64_t           start_timestamp,
                            uint64_t           end_timestamp) {
  uint64_t start_aligned=start_timestamp - (start_timestamp%(3600*1000));
  char     dir[MAX_PATH_LEN];
  char     f[MAX_PATH_LEN];
  
  DBG<<"Searching from: "<<chaos::common::utility::TimingUtil::toString(start_timestamp)<<" to:"<<chaos::common::utility::TimingUtil::toString(end_timestamp);
  for (uint64_t start = start_aligned; start < end_timestamp; start += (3600*1000)) {
        calcFileDir(basedatapath, key,"", start, 0, 0, dir, f);
        boost::filesystem::path p(dir);
        if (boost::filesystem::exists(p) && is_directory(p)) {
          INFO<<"REMOVING "<<p;
          remove_all(p);

        }
  }
    
  return 0;
}
struct path_leaf_string
{
    std::string operator()(const boost::filesystem::directory_entry& entry) const
    {
        return entry.path().leaf().string();
    }
};
uint32_t PosixFile::countFromPath(boost::filesystem::path& p,const uint64_t timestamp_from,
                          const uint64_t timestamp_to){
    uint32_t elements=0;
    if (boost::filesystem::exists(p) && is_directory(p)) {
      std::vector<path> v;
      std::transform(directory_iterator(p), directory_iterator(), std::back_inserter(v), path_leaf_string());
      uint64_t iseq, irunid, tim;

      for (std::vector<path>::iterator it = v.begin(); it != v.end(); it++) {
        sscanf(it->c_str(), "%Lu_%llu_%llu", &tim, &irunid, &iseq);
        tim *= 1000;
        if ((tim < timestamp_to) && (tim >= timestamp_from)) {
          elements++;
      }

    }
    }
    return elements;
}

int PosixFile::getFromPath(boost::filesystem::path& p,const uint64_t timestamp_from,
                          const uint64_t timestamp_to,
                          const uint32_t page_len,
                          abstraction::VectorObject& found_object_page,
                          chaos::common::direct_io::channel::opcode_headers::SearchSequence& last_record_found_seq){
  uint64_t seqid    = last_record_found_seq.datapack_counter;
  uint64_t runid    = last_record_found_seq.run_id;
  int elements=0;
  
       if (boost::filesystem::exists(p) && is_directory(p)) {
          std::vector<path> v;
           std::transform(directory_iterator(p), directory_iterator(), std::back_inserter(v), path_leaf_string());

         // std::copy(directory_iterator(cdir), directory_iterator(), std::back_inserter(v));
          std::sort(v.begin(), v.end());
          for (std::vector<path>::iterator it = v.begin(); it != v.end(); it++) {
            uint64_t iseq, irunid, tim;
          
            sscanf(it->c_str(), "%Lu_%llu_%llu", &tim, &irunid, &iseq);
            tim *= 1000;
            if ((tim < timestamp_to) && (tim >= timestamp_from) && (irunid >= runid) && iseq >= seqid) {
              char tmpbuf[MAX_PATH_LEN];
              snprintf(tmpbuf,sizeof(tmpbuf),"%s/%s",p.c_str(),it->c_str());
              ifstream infile(tmpbuf, std::ofstream::binary);
              infile.seekg(0, infile.end);
              long size = infile.tellg();
              infile.seekg(0);
              char* buffer = new char[size];
              infile.read(buffer, size);
            //  DBG << "retriving \"" << *it << "\" seq:" << iseq << " runid:" << irunid << " data size:" << size;
              chaos::common::data::CDWShrdPtr new_obj(new chaos::common::data::CDataWrapper((const char*)buffer, size));

              found_object_page.push_back(new_obj);
              delete buffer;
              infile.close();
              last_record_found_seq.run_id           = irunid;
              last_record_found_seq.datapack_counter = iseq;
              elements++;
              if(page_len>0 && (elements>=page_len)){
                return elements;
              }
            }
          }
        }
        return elements;
 
}
//!search object into object persistence layer
int PosixFile::findObject(const std::string&                                                 key,
                          const ChaosStringSet&                                              meta_tags,
                          const ChaosStringSet&                                              projection_keys,
                          const uint64_t                                                     timestamp_from,
                          const uint64_t                                                     timestamp_to,
                          const uint32_t                                                     page_len,
                          abstraction::VectorObject&                                                      found_object_page,
                          chaos::common::direct_io::channel::opcode_headers::SearchSequence& last_record_found_seq) {
  int err = 0;

  uint64_t seqid    = last_record_found_seq.datapack_counter;
  uint64_t runid    = last_record_found_seq.run_id;
  int      old_hour = -1;
  char     dir[MAX_PATH_LEN];
  char     f[MAX_PATH_LEN];
  dir[0] = 0;
  int elements=0;
  try {
     std::string tag;
     {
      if(meta_tags.size()>0){

        //tag=std::accumulate(meta_tags.begin(),meta_tags.end(),std::string("_"));
        tag = boost::algorithm::join(meta_tags,"_");
      }
      std::string st=chaos::common::utility::TimingUtil::toString(timestamp_from);
      std::string sto=chaos::common::utility::TimingUtil::toString(timestamp_to);
      DBG << "Search "<<key.c_str()<<" from:"<<st<<" to:"<< sto<< " seq:" << seqid << " runid:" << runid << " TAGS:"<<tag;

     }

      if(meta_tags.size()>0){
        calcFileDir(basedatapath, key, tag, timestamp_from, seqid, runid, dir, f);

        boost::filesystem::path p(dir);
        if (boost::filesystem::exists(p) && is_directory(p)) {
            elements+=getFromPath(p,timestamp_from,timestamp_to,page_len,found_object_page,last_record_found_seq);
        } else {
          ERR<<"DIR TAG NOT FOUND:"<<dir;
          return -1;
        }
        return 0;

    }
    // align to hour
    uint64_t start_aligned=timestamp_from - (timestamp_from%(3600*1000));
    for (uint64_t start = start_aligned; start < timestamp_to; start += (3600*1000)) {
      time_t     t     = (start / 1000);
      struct tm* tinfo = localtime(&t);
      if (tinfo &&(tinfo->tm_hour != old_hour)) {
        calcFileDir(basedatapath, key,tag, start, seqid, runid, dir, f);
        boost::filesystem::path p(dir);
        DBG << "["<<ctime(&t)<<"] Looking in \"" << dir << "\" seq:" << seqid << " runid:" << runid;

        elements+=getFromPath(p,timestamp_from,timestamp_to,page_len,found_object_page,last_record_found_seq);
        if(elements>=page_len){
          DBG <<"["<< p<<"] Found "<<elements<<" page:"<<page_len<< " last runid:"<<last_record_found_seq.run_id<<" last seq:"<<last_record_found_seq.datapack_counter;

          return 0;
        }
        old_hour = tinfo->tm_hour;
      }
    }

  } catch (const std::exception e) {
    ERR << e.what();
  }
  return err;
}

//!fast search object into object persistence layer
/*!
                     Fast search return only data index to the client, in this csae client ned to use api to return the single
                     or grouped data
                     */
int PosixFile::findObjectIndex(const abstraction::DataSearch&                                                  search,
                               abstraction::VectorObject&                                                      found_object_page,
                               chaos::common::direct_io::channel::opcode_headers::SearchSequence& last_record_found_seq) {
  ERR << " NOT IMPLEMENTED";

  return 0;
}

//! return the object asosciated with the index array
/*!
                     For every index object witl be returned the associated data object, if no data is received will be
                     insert an empty object
                     */
int PosixFile::getObjectByIndex(const chaos::common::data::CDWShrdPtr& index,
                                chaos::common::data::CDWShrdPtr&       found_object) {
  ERR << " NOT IMPLEMENTED";

  return 0;
}

//!return the number of object for a determinated key that are store for a time range
int PosixFile::countObject(const std::string& key,
                           const uint64_t     timestamp_from,
                           const uint64_t     timestamp_to,
                          uint64_t&    object_count) {

  int      old_hour = -1;
  char     dir[MAX_PATH_LEN];
  char     f[MAX_PATH_LEN];
  dir[0] = 0;
    object_count=0;
    chaos::common::direct_io::channel::opcode_headers::SearchSequence last_record_found_seq;
    last_record_found_seq.datapack_counter=0;
    last_record_found_seq.run_id=0;
    
    uint64_t start_aligned=timestamp_from - (timestamp_from%(3600*1000));
    for (uint64_t start = start_aligned; start < timestamp_to; start += (3600*1000)) {
      time_t     t     = (start / 1000);
      struct tm* tinfo = localtime(&t);
      if (tinfo &&(tinfo->tm_hour != old_hour)) {
        calcFileDir(basedatapath, key,"", start, 0, 0, dir, f);
        boost::filesystem::path p(dir);
        
        object_count+=countFromPath(p,timestamp_from,timestamp_to);
        old_hour = tinfo->tm_hour;
      }
    }
  return 0;
}

}  // namespace object_storage
}  // namespace metadata_service
}  // namespace chaos
