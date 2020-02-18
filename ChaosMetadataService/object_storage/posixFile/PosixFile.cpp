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
#include <chaos/common/configuration/GlobalConfiguration.h>
#include <boost/algorithm/string/join.hpp>
#include <boost/filesystem.hpp>
#include <boost/regex.hpp>
#define INFO INFO_LOG(PosixFile)
#define DBG DBG_LOG(PosixFile)
#define ERR ERR_LOG(PosixFile)
#include <chaos/common/utility/TimingUtil.h>
using namespace chaos::metadata_service::object_storage;
using namespace boost::filesystem;
#define MAX_PATH_LEN 512
#if CHAOS_PROMETHEUS
using namespace chaos::common::metric;
#endif
using namespace chaos::common::async_central;

namespace chaos {
namespace metadata_service {
namespace object_storage {

PosixFile::PosixFile(const std::string& name)
    : basedatapath(name) {
  serverName = "cds_" + chaos::GlobalConfiguration::getInstance()->getHostname();

#if CHAOS_PROMETHEUS
  MetricManager::getInstance()->createCounterFamily("mds_storage_io_data", "Measure the data rate for the data sent and read from object storage [byte]");
  counter_write_data_uptr = MetricManager::getInstance()->getNewCounterFromFamily("mds_storage_io_data", {{"type", "write_byte"}});
  counter_read_data_uptr  = MetricManager::getInstance()->getNewCounterFromFamily("mds_storage_io_data", {{"type", "read_byte"}});

  MetricManager::getInstance()->createGaugeFamily("mds_storage_op_time", "Measure the time spent by object storageto complete operation [milliseconds]");
  gauge_insert_time_uptr = MetricManager::getInstance()->getNewGaugeFromFamily("mds_storage_op_time", {{"type", "insert_time"}});
  gauge_query_time_uptr  = MetricManager::getInstance()->getNewGaugeFromFamily("mds_storage_op_time", {{"type", "query_time"}});
  DBG << " CREATED METRICS";

#endif
  DBG << " BASED DIR:" << name;
  AsyncCentralManager::getInstance()->addTimer(this, 5000, 5000);
}

PosixFile::~PosixFile() {}
PosixFile::write_path_t PosixFile::s_lastWriteDir;
std::string             PosixFile::serverName;
PosixFile::cacheRead_t  PosixFile::s_lastAccessedDir;
PosixFile::ordered_t PosixFile::s_ordered;

//ChaosSharedMutex PosixFile::devio_mutex;
ChaosSharedMutex PosixFile::last_access_mutex;

void PosixFile::calcFileDir(const std::string& prefix, const std::string& cu, const std::string& tag, uint64_t ts_ms, uint64_t seq, uint64_t runid, char* dir, char* fname) {
  // std::size_t found = cu.find_last_of("/");
  //time_t     t     = (ts_ms==0)?time(NULL):(ts_ms / 1000);
  time_t    t = (ts_ms / 1000);
  struct tm tinfo;
  localtime_r(&t, &tinfo);

  // CU PATH NAME/<yyyy>/<mm>/<dd>/<hour>
  if (tag.size() > 0) {
    snprintf(dir, MAX_PATH_LEN, "%s/%s/%s", prefix.c_str(), cu.c_str(), tag.c_str());

  } else {
    snprintf(dir, MAX_PATH_LEN, "%s/%s/%.4d/%.2d/%.2d/%.2d/%.2d", prefix.c_str(), cu.c_str(), tinfo.tm_year + 1900, tinfo.tm_mon + 1, tinfo.tm_mday, tinfo.tm_hour, tinfo.tm_min);
  }
  // timestamp_runid_seq_ssss
  snprintf(fname, MAX_PATH_LEN, "%llu_%llu_%.10llu", ts_ms, runid, seq);
}
static void*
test_bson_writer_custom_realloc_helper(void* mem, size_t num_bytes, void* ctx) {
  boost::iostreams::mapped_file* mf = (boost::iostreams::mapped_file*)ctx;
  mf->resize(num_bytes);

  return mf->data();
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
  const uint64_t ts = stored_object.getInt64Value(NodeHealtDefinitionKey::NODE_HEALT_MDS_TIMESTAMP);
  char           dir[MAX_PATH_LEN];
  char           f[MAX_PATH_LEN];
  uint8_t*       buf;
  size_t         buflen;
  int64_t        seq, runid;
  std::string    tag;
  if (meta_tags->size() > 0) {
    //tag=std::accumulate(meta_tags->begin(),meta_tags->end(),std::string("_"));
    tag = boost::algorithm::join(*meta_tags.get(), "_");
  }
  seq   = stored_object.getInt64Value(chaos::DataPackCommonKey::DPCK_SEQ_ID);
  runid = stored_object.getInt64Value(chaos::ControlUnitDatapackCommonKey::RUN_ID);
  calcFileDir(basedatapath, key, tag, ts, seq, runid, dir, f);
  const write_path_t::iterator id = s_lastWriteDir.find(dir);
  if ((id == s_lastWriteDir.end()) || (ts - (id->second).ts) > 1000) {
    boost::filesystem::path p(dir);
    if ((boost::filesystem::exists(p) == false)) {
      if ((boost::filesystem::create_directories(p) == false)) {
        ERR << "cannot create directory:" << p;
        return -1;
      } else {
        ChaosWriteLock                       ll(s_lastWriteDir[dir].devio_mutex);
        boost::iostreams::mapped_file_params params;
        params.path                    = std::string(dir) + "/" + serverName;
        params.new_file_size           = stored_object.getBSONRawSize() * 100;
        params.flags                   = boost::iostreams::mapped_file::mapmode::readwrite;
        s_lastWriteDir[dir].size       = 0;
        s_lastWriteDir[dir].nobj       = 0;
        s_lastWriteDir[dir].last_seq   = 0;
        s_lastWriteDir[dir].last_runid = 0;

        s_lastWriteDir[dir].max_size = params.new_file_size;

        s_lastWriteDir[dir].mf.open(params);
        if (s_lastWriteDir[dir].mf.is_open()) {
          s_lastWriteDir[dir].fname = params.path;

          DBG << " CREATED DIR:" << p << " mapped file:" << params.path << " max size:" << params.new_file_size;
        } else {
          ERR << "cannot open "
              << " mapped file:" << params.path << " max size:" << params.new_file_size;
          return -2;
        }
        buf                        = (uint8_t*)s_lastWriteDir[dir].mf.data();
        buflen                     = params.new_file_size;
        s_lastWriteDir[dir].writer = bson_writer_new(&buf, &buflen, 0, test_bson_writer_custom_realloc_helper, &s_lastWriteDir[dir].mf);
      }
    }
    s_lastWriteDir[dir].ts = ts;
  }
  ChaosWriteLock ll(s_lastWriteDir[dir].devio_mutex);

  if (s_lastWriteDir[dir].mf.is_open()) {
    /*if(s_lastWriteDir[dir].size*2>s_lastWriteDir[dir].max_size){
                s_lastWriteDir[dir].mf.resize(s_lastWriteDir[dir].max_size*2);
                s_lastWriteDir[dir].max_size*=2;
              }*/
    bson_t* b;
    if(bson_writer_begin(s_lastWriteDir[dir].writer, &b)){

    char key[24];
    sprintf(key, "%llu_%.10llu", runid, seq);
    bson_append_document(b, key, -1, stored_object.getBSON());
    /*  if(s_lastWriteDir[dir].last_seq>0 && (seq!=(s_lastWriteDir[dir].last_seq+1))){
                  DBG<<"MISSING seq:"<<seq<<" last_seq:"<<s_lastWriteDir[dir].last_seq<<" diff:"<<(seq-s_lastWriteDir[dir].last_seq);
                }*/
    
    //                char* str = bson_as_canonical_extended_json (b, NULL);
    //                DBG<<":"<<str;
    //                bson_free(str);
    bson_writer_end(s_lastWriteDir[dir].writer);
    s_lastWriteDir[dir].last_seq   = seq;
    s_lastWriteDir[dir].last_runid = runid;
    s_lastWriteDir[dir].nobj++;

    s_lastWriteDir[dir].keys.push_back(key);

#if CHAOS_PROMETHEUS

    (*counter_write_data_uptr) += stored_object.getBSONRawSize();
#endif
    }
  } else {
    ERR << " CANNOT WRITE:" << dir;
    return -2;
  }

#if CHAOS_PROMETHEUS

  (*gauge_insert_time_uptr) = (chaos::common::utility::TimingUtil::getTimeStamp() - ts);
#endif
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
int PosixFile::removeRecursevelyUp(const boost::filesystem::path& p) {
  if (p == basedatapath) {
    return 0;
  }

  if (boost::filesystem::is_empty(p.parent_path())) {
    if (remove_all(p.parent_path())) {
      INFO << "REMOVING " << p.parent_path();

      return removeRecursevelyUp(p.parent_path());
    }
  }
  return 0;
}

//!delete objects that are contained between intervall (exstreme included)
int PosixFile::deleteObject(const std::string& key,
                            uint64_t           start_timestamp,
                            uint64_t           end_timestamp) {
  uint64_t start_aligned = start_timestamp - (start_timestamp % (3600 * 1000));
  char     dir[MAX_PATH_LEN];
  char     f[MAX_PATH_LEN];

  DBG << "Searching \"" << key << "\" from: " << chaos::common::utility::TimingUtil::toString(start_timestamp) << " to:" << chaos::common::utility::TimingUtil::toString(end_timestamp);
  for (uint64_t start = start_aligned; start < end_timestamp; start += (3600 * 1000)) {
    calcFileDir(basedatapath, key, "", start, 0, 0, dir, f);
    boost::filesystem::path p(dir);
    if (boost::filesystem::exists(p) && is_directory(p)) {
      INFO << "REMOVING " << p;
      if (remove_all(p) > 0) {
        removeRecursevelyUp(p);
      }

      /* std::map<std::string,uint64_t>::iterator id=s_lastDirs.find(dir);
          if(id!=s_lastDirs.end()){
            s_lastDirs.erase(id);
          }*/
    }
  }

  return 0;
}
struct path_leaf_string {
  std::string operator()(const boost::filesystem::directory_entry& entry) const {
    return entry.path().leaf().string();
  }
};
uint32_t PosixFile::countFromPath(boost::filesystem::path& p, const uint64_t timestamp_from, const uint64_t timestamp_to) {
  uint32_t elements = 0;
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
std::vector<boost::filesystem::path> searchFile(boost::filesystem::path& p,const boost::regex& my_filter){
  std::vector< boost::filesystem::path> all_matching_files;

boost::filesystem::directory_iterator end_itr; // Default ctor yields past-the-end
for( boost::filesystem::directory_iterator i( p ); i != end_itr; ++i )
{
    // Skip if not a file
    if( !boost::filesystem::is_regular_file( i->status() ) ) continue;

    boost::smatch what;

    // Skip if no match for V2:
    if( !boost::regex_match( i->path().filename().string(), what, my_filter ) ) continue;
    // For V3:
    //if( !boost::regex_match( i->path().filename().string(), what, my_filter ) ) continue;

    // File matches, store it
    all_matching_files.push_back( *i );
}
  return all_matching_files;
}

std::vector<std::string> searchFileExt(const std::string& pa,const std::string& ext){
  std::vector< std::string> all_matching_files;
boost::filesystem::path p(pa);
boost::filesystem::directory_iterator end_itr; // Default ctor yields past-the-end
for( boost::filesystem::directory_iterator i( p ); i != end_itr; ++i )
{
    // Skip if not a file
  
    if( !boost::filesystem::is_regular_file( i->status() ) ) continue;
    if(i->path().filename().extension()==ext){
          all_matching_files.push_back( i->path().string() );

    }
}
  return all_matching_files;
}

int PosixFile::getFromPath(const std::string& dir, const uint64_t timestamp_from, const uint64_t timestamp_to, const uint32_t page_len, abstraction::VectorObject& found_object_page, chaos::common::direct_io::channel::opcode_headers::SearchSequence& last_record_found_seq) {
  uint64_t seqid    = last_record_found_seq.datapack_counter;
  uint64_t runid    = last_record_found_seq.run_id;
  int      elements = 0;
  //   const cacheRead_t::iterator di=s_lastAccessedDir.find(dir);
  //  if(di==s_lastAccessedDir.end()){
  boost::filesystem::path p(dir+"/"+ serverName+".ordered");
  if (!boost::filesystem::exists(p)||(s_ordered.find(p.string())!=s_ordered.end())) {
    // se non esiste oppure e' stato gia' fatto un ordinamento temporaneo
    ChaosWriteLock(s_ordered[p.string()].devio_mutex);

    write_path_t::iterator id=s_lastWriteDir.find(dir);
    if(id!=s_lastWriteDir.end()){
        id->second.devio_mutex.lock();
        int len = bson_writer_get_length(id->second.writer);
        DBG << " FORCE LOCAL ORDERING OF:" << p<<" objects:"<<id->second.nobj;

        std::sort(id->second.keys.begin(), id->second.keys.end());
        int retw=reorderData(p.string(),id->second.keys,len,id->second.fname);
        DBG << " REORDERED:" << retw;
        s_ordered[p.string()].obj=retw;
        id->second.devio_mutex.unlock();

    }
  }
  std::vector<std::string> fr=searchFileExt(dir,".ordered");

 for(std::vector<std::string>::iterator po=fr.begin();po!=fr.end();po++){
   ordered_t::iterator oi=s_ordered.find(*po);
   if(oi!=s_ordered.end()){
     oi->second.devio_mutex.lock();
   }
    //boost::iostreams::mapped_file_params params;
    // params.path=path;
    //params.flags =boost::iostreams::mapped_file::mapmode::readonly;
    try {
      boost::iostreams::mapped_file_source mf;
      mf.open(*po);
      if (mf.is_open()) {
        bson_reader_t*    reader;
        bson_iter_t       it;
        const bson_t*     bson;
        bson_iter_t       iter;
        bool              end;
        reader = bson_reader_new_from_data((const uint8_t*)mf.data(), (uint32_t)mf.size());
        /*  while((bson=bson_reader_read(reader,&end))){
                            char* str = bson_as_canonical_extended_json (bson, NULL);
                DBG<<":"<<str;
                bson_free(str);

            }*/
        //  const bson_t* bson= bson_new_from_data ((const uint8_t*)mf.data(), (uint32_t) mf.size());
        //  chaos::common::data::CDataWrapper data(bson);

   //     DBG << "Found path " << *po << " len:" << mf.size() << " looking keys runid>="<<runid<<" seq>="<<seqid<<" ts<"<<timestamp_to<<" ts>="<<timestamp_from;
        int count=0;
        while ((bson = bson_reader_read(reader, &end))) {
          bson_iter_t iter;
          bson_iter_init(&iter, bson);
          uint32_t       document_len = 0;
          const uint8_t* document     = NULL;
          bson_iter_next(&iter);
          bson_iter_document(&iter,
                             &document_len,
                             &document);
          chaos::common::data::CDWShrdPtr d(new chaos::common::data::CDataWrapper((const char*)document, document_len));
          uint64_t                        iseq, irunid, tim;
          iseq   = d->getInt64Value(chaos::DataPackCommonKey::DPCK_SEQ_ID);
          irunid = d->getInt64Value(chaos::ControlUnitDatapackCommonKey::RUN_ID);
          tim    = d->getInt64Value(NodeHealtDefinitionKey::NODE_HEALT_MDS_TIMESTAMP);
   /*       if((count++%1000)==0){
            DBG << count<<"]"<<(((tim < timestamp_to) && (tim >= timestamp_from) && (irunid >= runid) && (iseq >= seqid))?"OK":"NOK")<<" KEY: " << bson_iter_key(&iter) << " seq:" << iseq << "("<<(iseq >= seqid)<<") run:"<<irunid<< "("<<(irunid >= runid)<<"} ts:"<<tim<<"("<<(tim < timestamp_to)<<","<<(tim >= timestamp_from)<<")";
          }*/
          if ((tim < timestamp_to) && (tim >= timestamp_from) && (irunid >= runid) && (iseq >= seqid)) {
            found_object_page.push_back(d);
            last_record_found_seq.run_id           = irunid;
            last_record_found_seq.datapack_counter = iseq;
            elements++;
#if CHAOS_PROMETHEUS

            (*counter_read_data_uptr) += d->getBSONRawSize();
#endif

            if (page_len > 0 && (elements >= page_len)) {
              mf.close();
              if(oi!=s_ordered.end()){
                  oi->second.devio_mutex.unlock();
                }
              return elements;
            }
          } else {
            
          }
        }
      } else {
        ERR<<"cannot open "<<*po;
      }
    } catch (std::exception& e) {
      ERR << "Exception occur:" << e.what();
    }
  if(oi!=s_ordered.end()){
      oi->second.devio_mutex.unlock();
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
                          abstraction::VectorObject&                                         found_object_page,
                          chaos::common::direct_io::channel::opcode_headers::SearchSequence& last_record_found_seq) {
  int err = 0;

  uint64_t seqid    = last_record_found_seq.datapack_counter;
  uint64_t runid    = last_record_found_seq.run_id;
  int      old_hour = -1;
  char     dir[MAX_PATH_LEN];
  char     f[MAX_PATH_LEN];
  dir[0]            = 0;
  const uint64_t ts = chaos::common::utility::TimingUtil::getTimeStamp();

  int elements = 0;
  try {
    std::string tag;
    {
      if (meta_tags.size() > 0) {
        //tag=std::accumulate(meta_tags.begin(),meta_tags.end(),std::string("_"));
        tag = boost::algorithm::join(meta_tags, "_");
      }
      std::string st  = chaos::common::utility::TimingUtil::toString(timestamp_from);
      std::string sto = chaos::common::utility::TimingUtil::toString(timestamp_to);
      DBG << "Search " << key.c_str() << " from:" << st << " to:" << sto << " seq:" << seqid << " runid:" << runid << " TAGS:" << tag;
    }

    if (meta_tags.size() > 0) {
      calcFileDir(basedatapath, key, tag, timestamp_from, seqid, runid, dir, f);

      elements += getFromPath(dir, timestamp_from, timestamp_to, page_len, found_object_page, last_record_found_seq);

      return 0;
    }
    // align to hour
    uint64_t start_aligned = timestamp_from - (timestamp_from % (60 * 1000));
    for (uint64_t start = start_aligned; start < timestamp_to; start += (60 * 1000)) {
      time_t t = (start / 1000);
      //struct tm* tinfo = localtime(&t);
      struct tm tinfo;
      localtime_r(&t, &tinfo);
      if ((tinfo.tm_min != old_hour)) {
        calcFileDir(basedatapath, key, tag, start, seqid, runid, dir, f);
        // boost::filesystem::path p(dir);
        DBG << "[" <<chaos::common::utility::TimingUtil::toString(start) << "] Looking in \"" << dir << "\" seq:" << seqid << " runid:" << runid;

        elements += getFromPath(dir, timestamp_from, timestamp_to, page_len, found_object_page, last_record_found_seq);
        if (elements >= page_len) {
          DBG << "[" << dir << "] Found " << elements << " page:" << page_len << " last runid:" << last_record_found_seq.run_id << " last seq:" << last_record_found_seq.datapack_counter;
#if CHAOS_PROMETHEUS

          (*gauge_query_time_uptr) = (chaos::common::utility::TimingUtil::getTimeStamp() - ts);
#endif
          return 0;
        } else if(elements==0){
          DBG << "[" << dir << "] NO ELEMENTS FOUND last runid:" << last_record_found_seq.run_id << " last seq:" << last_record_found_seq.datapack_counter;

        }
        old_hour = tinfo.tm_min;
      }
    }

  } catch (const std::exception e) {
    ERR << e.what();
  }
  if (err == 0 && elements > 0) {
#if CHAOS_PROMETHEUS

    (*gauge_query_time_uptr) = (chaos::common::utility::TimingUtil::getTimeStamp() - ts);
#endif
  }
  return err;
}

//!fast search object into object persistence layer
/*!
                     Fast search return only data index to the client, in this csae client ned to use api to return the single
                     or grouped data
                     */
int PosixFile::findObjectIndex(const abstraction::DataSearch&                                     search,
                               abstraction::VectorObject&                                         found_object_page,
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
int PosixFile::reorderData(const std::string& dstpath, const std::vector<std::string>& keys, int size, const std::string& srcpath) {
  int            err = 0;
  bson_error_t   src_err;
  bson_reader_t* src = bson_reader_new_from_file(srcpath.c_str(), &src_err);
  if (src == NULL) {
    ERR << " cannot open for read or not valid:" << srcpath;
    return -1;
  }
  if (size <= 0) {
    // calc size;
    std::ifstream ifp(srcpath, std::ifstream::ate | std::ifstream::binary);
    size = ifp.tellg();
  }
  boost::iostreams::mapped_file_params params;
  params.path          = dstpath;
  params.new_file_size = size;
  params.flags         = boost::iostreams::mapped_file::mapmode::readwrite;
  boost::iostreams::mapped_file mf(params);
  if (mf.is_open() == false) {
    ERR << " cannot map/open for write:" << dstpath;
    return -2;
  }
  uint8_t* buf    = (uint8_t*)mf.data();
  size_t   buflen = size;

  bson_writer_t*                           dst = bson_writer_new(&buf, &buflen, 0, test_bson_writer_custom_realloc_helper, &mf);
  std::map<std::string, bson_value_t>      toreorder;
  bool                                     eof;
  std::vector<std::string>::const_iterator i = keys.begin();
  const bson_t*                            b;
  int                                      ritems = 0, witems = 0;
  
  while (((b = bson_reader_read(src, &eof)) != NULL)&&(eof==false)) {
    bson_iter_t iter;
    bson_iter_init(&iter, b);
    bson_iter_next(&iter);
    const char* pkey=bson_iter_key(&iter);
        ritems++;

    if(pkey==NULL){
      ERR<< ritems<<","<<witems<<"] NULL KEY";
      continue;
    }

    std::string key(pkey) ;
    const bson_value_t* val = bson_iter_value(&iter);
    if (*i == key) {
      bson_t* wb;
      // trovata ordinata
      bson_writer_begin(dst, &wb);
      bson_append_value(wb, key.c_str(), -1, val);
      bson_writer_end(dst);
      witems++;
     // DBG << ritems<<","<<witems<<"] found ordered " << *i<< " size:"<<toreorder.size();
      i++;

    } else {
      // guarda tra quelle disordinate accumulate
      std::map<std::string, bson_value_t>::iterator f;
      while(((f = toreorder.find(*i))!=toreorder.end())&&(i!=keys.end())){
        bson_t* wb;
        bson_writer_begin(dst, &wb);
        bson_append_value(wb, i->c_str(), -1, &f->second);
        bson_writer_end(dst);
        bson_value_destroy(&f->second);
        toreorder.erase(f);
        witems++;
      //  DBG << ritems<<","<<witems<<"] found into reordered " << *i<< " size:"<<toreorder.size();

        i++;
      }
        if((i!=keys.end() )&&(*i==key)){
          bson_t* wb;
      // trovata ordinata
          bson_writer_begin(dst, &wb);
          bson_append_value(wb, key.c_str(), -1, val);
          bson_writer_end(dst);
          witems++;
          DBG << ritems<<","<<witems<<"] re-found ordered " << *i<< " size:"<<toreorder.size();
          i++;
        } else {
          bson_value_t& copy = toreorder[key];
      //    DBG << ritems<<","<<witems<<"] searching " << *i << " found :" << key << " item to reorder:" << toreorder.size();
        
          bson_value_copy(val, &copy);
        }
      }
  }
  DBG << srcpath<<"] READ ITEMS:" << ritems << " " <<dstpath<<"] WRITTEN:" << witems;
  if (ritems != witems) {
    ERR << " W/R ITEMS mismatch:";
  }

  bson_writer_destroy(dst);
  bson_reader_destroy(src);
  mf.close();

  
  return witems;
}
void PosixFile::timeout() {
  uint64_t ts = chaos::common::utility::TimingUtil::getTimeStamp();

  // remove directory write cache
  for (write_path_t::iterator id = s_lastWriteDir.begin(); id != s_lastWriteDir.end(); id) {
    if ((ts - id->second.ts) > 60000) {
      //not anymore used
      id->second.devio_mutex.lock();
      if (id->second.mf.is_open()) {
        int len = bson_writer_get_length(id->second.writer);
        DBG << "Closing and resing " << id->second.fname << " with nobj:" << id->second.nobj << " from:" << id->second.mf.size() << " resizing to:" << len;
        id->second.mf.resize((boost::iostreams::mapped_file::size_type)len);
        bson_writer_destroy(id->second.writer);
        id->second.mf.close();
        // sort keys
        id->second.writer = NULL;  //mark closed.
        

        std::string dstpath=id->second.fname+".ordered";
        ordered_t::iterator oi=s_ordered.find(dstpath);
        if(oi!=s_ordered.end()){
          oi->second.devio_mutex.lock();
          if(oi->second.obj==id->second.nobj){
             DBG << " Already ordered... skipping";
            if(boost::filesystem::remove(boost::filesystem::path(id->second.fname))){
                  DBG << " REMOVED:" << id->second.fname;

              } else {
                ERR<<" cannot remove:"<<id->second.fname;
              }
              id->second.devio_mutex.unlock();
              s_lastWriteDir.erase(id++);
              s_ordered.erase(oi);
              continue;
          }
        }
        std::sort(id->second.keys.begin(), id->second.keys.end());

        /// reorder data
        int retw=reorderData(dstpath,id->second.keys,len,id->second.fname);
        if(retw==id->second.nobj )
          if(boost::filesystem::remove(boost::filesystem::path(id->second.fname))){
            DBG << " REMOVED:" << id->second.fname;

         } else {
          ERR<<" cannot remove:"<<id->second.fname;
        }
        if(oi!=s_ordered.end()){
          oi->second.devio_mutex.unlock();

      }
      } else {
        ERR << "Not closed because waw not open " << id->second.fname << " with nobj:" << id->second.nobj << " from:" << id->second.mf.size();
      }
      id->second.devio_mutex.unlock();

      s_lastWriteDir.erase(id++);
      

    } else {
      id++;
    }
  }
  for (cacheRead_t::iterator id = s_lastAccessedDir.begin(); id != s_lastAccessedDir.end(); id) {
    if ((ts - id->second.ts) > 5000) {
      //not anymore used
      if (id->second.devio_mutex.try_lock()) {
        {
          DBG << "cleanup " << id->first;
          ChaosWriteLock ll(last_access_mutex);
          id->second.devio_mutex.unlock();
          s_lastAccessedDir.erase(id++);
          DBG << "end cleanup ";
        }
      }
    } else {
      id++;
    }
  }
}

//!return the number of object for a determinated key that are store for a time range
int PosixFile::countObject(const std::string& key,
                           const uint64_t     timestamp_from,
                           const uint64_t     timestamp_to,
                           uint64_t&          object_count) {
  int  old_hour = -1;
  char dir[MAX_PATH_LEN];
  char f[MAX_PATH_LEN];
  dir[0]       = 0;
  object_count = 0;
  chaos::common::direct_io::channel::opcode_headers::SearchSequence last_record_found_seq;
  last_record_found_seq.datapack_counter = 0;
  last_record_found_seq.run_id           = 0;

  uint64_t start_aligned = timestamp_from - (timestamp_from % (3600 * 1000));
  for (uint64_t start = start_aligned; start < timestamp_to; start += (3600 * 1000)) {
    time_t    t = (start / 1000);
    struct tm tinfo;
    localtime_r(&t, &tinfo);
    if ((tinfo.tm_hour != old_hour)) {
      calcFileDir(basedatapath, key, "", start, 0, 0, dir, f);
      boost::filesystem::path p(dir);

      object_count += countFromPath(p, timestamp_from, timestamp_to);
      old_hour = tinfo.tm_hour;
    }
  }
  return 0;
}

}  // namespace object_storage
}  // namespace metadata_service
}  // namespace chaos
