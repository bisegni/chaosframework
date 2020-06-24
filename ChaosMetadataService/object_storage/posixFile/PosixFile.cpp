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
#include "lz4_stream.h"
#ifdef CERN_ROOT
#include <driver/misc/models/cernRoot/rootUtil.h>
#include "TFile.h"
#include "TList.h"
#endif

#if CHAOS_PROMETHEUS
using namespace chaos::common::metric;
#endif
using namespace chaos::common::async_central;

#define MAXDIROPENED 1000
namespace chaos {
namespace metadata_service {
namespace object_storage {
#if CHAOS_PROMETHEUS

/*static global*/
chaos::common::metric::CounterUniquePtr PosixFile::counter_write_data_uptr;
chaos::common::metric::CounterUniquePtr PosixFile::counter_read_data_uptr;
chaos::common::metric::GaugeUniquePtr   PosixFile::gauge_insert_time_uptr;
chaos::common::metric::GaugeUniquePtr   PosixFile::gauge_query_time_uptr;
#endif
boost::lockfree::queue<PosixFile::dirpath_t*, boost::lockfree::fixed_sized<true> > PosixFile::file_to_finalize(MAXDIROPENED);
#ifdef CERN_ROOT
GenerateRootJob PosixFile::rootGenJob;
#endif
bool PosixFile::removeTemp   = false;
bool PosixFile::generateRoot = false;
bool PosixFile::compress     = false;
/**************/

static int lz4decomp(const std::string& src, const std::string& dst) {
  FileLock fl(dst + ".lock");
  fl.lock();
  if (boost::filesystem::exists(dst)) {
    fl.unlock();
    return 0;
  }

  try {
    std::ifstream       in_file(src);
    std::ofstream       out_file(dst + ".tmp");
    lz4_stream::istream lz4_stream(in_file);

    std::copy(std::istreambuf_iterator<char>(lz4_stream),
              std::istreambuf_iterator<char>(),
              std::ostreambuf_iterator<char>(out_file));
    boost::filesystem::rename(dst + ".tmp", dst);

  } catch (boost::filesystem::filesystem_error& e) {
    ERR << "filesystem error  " << e.what();
    return -2;

  } catch (std::exception& e) {
    ERR << "decompressing file " << src;
    return -1;
  }
  fl.unlock();
  return 0;
}
static int lz4compress(const std::string& src, const std::string& dst) {
  std::ifstream in_file(src);
  std::ofstream out_file(dst);
  try {
    lz4_stream::ostream lz4_stream(out_file);

    std::copy(std::istreambuf_iterator<char>(in_file),
              std::istreambuf_iterator<char>(),
              std::ostreambuf_iterator<char>(lz4_stream));
  } catch (std::exception e) {
    ERR << " error compressing " + src + " into " + dst;
    return -1;
  }
  return 0;
}
std::vector<std::string> searchFile(const std::string& p, const boost::regex& my_filter) {
  std::vector<std::string> all_matching_files;

  boost::filesystem::directory_iterator end_itr;  // Default ctor yields past-the-end
  for (boost::filesystem::directory_iterator i(p); i != end_itr; ++i) {
    // Skip if not a file
    if (!boost::filesystem::is_regular_file(i->status())) continue;

    boost::smatch what;

    // Skip if no match for V2:
    if (!boost::regex_match(i->path().filename().string(), what, my_filter)) continue;
    // For V3:
    //if( !boost::regex_match( i->path().filename().string(), what, my_filter ) ) continue;

    // File matches, store it
    all_matching_files.push_back(i->path().string());
  }
  return all_matching_files;
}

std::vector<std::string> searchFileExt(const std::string& pa, const std::string& ext) {
  std::vector<std::string>              all_matching_files;
  boost::filesystem::path               p(pa);
  boost::filesystem::directory_iterator end_itr;  // Default ctor yields past-the-end
  for (boost::filesystem::directory_iterator i(p); i != end_itr; ++i) {
    // Skip if not a file

    if (!boost::filesystem::is_regular_file(i->status())) continue;
    // DBG<<" comparing:"<<i->path().filename() <<" ext:"<<i->path().filename().extension()<<" with:"<<ext;
    if (i->path().filename().extension() == ext) {
      all_matching_files.push_back(i->path().string());
    }
  }
  return all_matching_files;
}

int reorderMulti(const std::string& dstpath, const std::vector<std::string>& srcs) {
  int            err = 0;
  bson_error_t   src_err;
  int            numsrc       = srcs.size();
  int            tot_exp_size = 0;
  bson_reader_t* src[numsrc];
  int            ritems[numsrc];

  for (int cnt = 0; cnt < numsrc; cnt++) {
    src[cnt] = bson_reader_new_from_file(srcs[cnt].c_str(), &src_err);
    if (src[cnt] == NULL) {
      ERR << " creating " << dstpath << " cannot open for read or not valid src [" << cnt << "]:" << srcs[cnt];
      while (cnt--) {
        bson_reader_destroy(src[cnt]);
      }
      return -1;
    }
    {
      std::ifstream ifp(srcs[cnt].c_str(), std::ifstream::ate | std::ifstream::binary);
      tot_exp_size += ifp.tellg();
    }
    ritems[cnt] = 0;
  }
  tot_exp_size = tot_exp_size + (tot_exp_size / 10);  // add 10%
  BsonFStream   dst(dstpath, tot_exp_size);
  bool          eof;
  const bson_t* b[numsrc];
  int           witems = 0, len;
  bson_iter_t*  iter   = new bson_iter_t[numsrc];

  do {
    std::vector<std::string>   keys_to_reorder;
    std::map<std::string, int> obj;

    for (int cnt = 0; cnt < numsrc; cnt++) {
      // leggi bson e keys
      if (((b[cnt] = bson_reader_read(src[cnt], &eof)) != NULL) && (eof == false)) {
        if (bson_iter_init(&iter[cnt], b[cnt])) {
          if (bson_iter_next(&iter[cnt])) {
            const char* pkey = bson_iter_key(&iter[cnt]);
            keys_to_reorder.push_back(pkey);
            obj[pkey] = cnt;
            ritems[cnt]++;
          }
        }
      }
    }
    //riordina chiavi
    std::sort(keys_to_reorder.begin(), keys_to_reorder.end());
    // scrivi chiavi riordinate
    for (std::vector<std::string>::const_iterator i = keys_to_reorder.begin(); i != keys_to_reorder.end(); i++) {
      dst.write(*i, bson_iter_value(&iter[obj[*i]]));

      witems++;
    }
    len = keys_to_reorder.size();
  } while (len > 0);
  delete[] iter;
  dst.close();
  for (int cnt = 0; cnt < numsrc; cnt++) {
    bson_reader_destroy(src[cnt]);
  }
  for (int cnt = 0; cnt < numsrc; cnt++) {
    DBG << srcs[cnt] << "] READ ITEMS:" << ritems[cnt] << " " << dstpath << "] WRITTEN:" << witems;
  }

  return witems;
}
int reorderData(const std::string& dstpath, const std::vector<std::string>& keys, int size, const std::string& srcpath) {
  int            err = 0;
  bson_error_t   src_err;
  bson_reader_t* src;

  if (keys.size() == 0) {
    ERR << "no keys to reorder..";
    return 0;
  }
  src = bson_reader_new_from_file(srcpath.c_str(), &src_err);
  if (src == NULL) {
    ERR << " cannot open for read or not valid:" << srcpath;
    return -1;
  }
  if (size <= 0) {
    // calc size;
    std::ifstream ifp(srcpath, std::ifstream::ate | std::ifstream::binary);
    size = ifp.tellg();
  }
  BsonFStream dst;
  if (dst.open(dstpath, size) != 0) {
    ERR << " cannot map/open for write:" << dstpath;
    return -2;
  }
  std::map<std::string, bson_value_t>      toreorder;
  bool                                     eof;
  std::vector<std::string>::const_iterator i = keys.begin();
  const bson_t*                            b;
  int                                      ritems = 0, witems = 0, rline = 0;

  while (((b = bson_reader_read(src, &eof)) != NULL) && (eof == false)) {
    bson_iter_t iter;

    rline++;
    if (bson_iter_init(&iter, b) == false) {
      ERR << srcpath << "] iter init false skipping " << rline;
      continue;
    }
    if (bson_iter_next(&iter) == false) {
      ERR << srcpath << "] iter next false skipping " << rline;
      continue;
    }
    const char* pkey = bson_iter_key(&iter);
    ritems++;

    if (pkey == NULL) {
      ERR << ritems << "," << witems << "] NULL KEY";
      continue;
    }

    std::string         key(pkey);
    const bson_value_t* val = bson_iter_value(&iter);
    if (val == NULL) {
      ERR << srcpath << "] no goot value found " << rline;
      continue;
    }
    if (*i == key) {
      bson_t* wb;
      // trovata ordinata
      dst.write(key, val);
      witems++;
      // DBG << ritems<<","<<witems<<"] found ordered " << *i<< " size:"<<toreorder.size();
      i++;

    } else {
      // guarda tra quelle disordinate accumulate
      std::map<std::string, bson_value_t>::iterator f;
      while (((f = toreorder.find(*i)) != toreorder.end()) && (i != keys.end())) {
        dst.write(*i, &f->second);
        bson_value_destroy(&f->second);
        toreorder.erase(f);
        witems++;
        //  DBG << ritems<<","<<witems<<"] found into reordered " << *i<< " size:"<<toreorder.size();

        i++;
      }
      if ((i != keys.end()) && (*i == key)) {
        // trovata ordinata
        dst.write(key, val);
        witems++;
        // DBG << ritems<<","<<witems<<"] re-found ordered " << *i<< " size:"<<toreorder.size();
        i++;
      } else {
        bson_value_t& copy = toreorder[key];
        //    DBG << ritems<<","<<witems<<"] searching " << *i << " found :" << key << " item to reorder:" << toreorder.size();

        bson_value_copy(val, &copy);
      }
    }
  }
  DBG << srcpath << "] READ ITEMS:" << ritems << " (" << rline << ") " << dstpath << "] WRITTEN:" << witems;
  if (ritems != witems || (ritems == 0)) {
    ERR << " W/R ITEMS mismatch OR ZERO READ";
  }

  bson_reader_destroy(src);
  return witems;
}

static int makeOrdered(PosixFile::fdw_t& second) {
  std::string             dstpath = second.fname + POSIX_ORDERED_EXT;
  boost::filesystem::path p(dstpath);
  FileLock                fl(dstpath + ".lock");
  fl.lock();
  uint64_t now = chaos::common::utility::TimingUtil::getTimeStamp();
  try {
    if ((!boost::filesystem::exists(p)) || (second.ordered_ts < second.unordered_ts)) {
      int retw = 0;

      if ((now - second.ts) > POSIX_MSEC_QUANTUM) {
        // no copy because non update any more
        std::sort(second.keys.begin(), second.keys.end());
        retw = reorderData(dstpath + ".tmp", second.keys, second.writer.size(), second.fname);
      } else {
        std::vector<std::string> ordered_keys = second.keys;
        std::sort(ordered_keys.begin(), ordered_keys.end());
        retw = reorderData(dstpath + ".tmp", ordered_keys, second.writer.size(), second.fname);
      }

      if (retw > 0) {
        boost::filesystem::rename(dstpath + ".tmp", dstpath);
        second.ordered_ts = now;
        return retw;
      } else {
        fl.unlock();
        ERR << " NOT REORDERED:" << second.fname << " size:" << second.writer.size() << " file :" << dstpath << " not created";
        return -1;
      }
    }
  } catch (boost::filesystem::filesystem_error& err) {
    fl.unlock();
    ERR << " filesystem error: " << err.what();
    return -10;
  }
  fl.unlock();
  return 0;  // not neet
}
#ifdef CERN_ROOT
static int createRoot(const std::string& srcpath, const std::string& dstpath) {
  boost::filesystem::path p(dstpath);
  int                     ret = 0;
  FileLock                fl(dstpath + ".lock");
  fl.lock();
  DBG << " creating " << dstpath << " from " << srcpath;
  if (boost::filesystem::is_regular_file(srcpath)) {
    bson_error_t src_err;

    bson_reader_t* src = bson_reader_new_from_file(srcpath.c_str(), &src_err);
    const bson_t*  b;
    bool           eof;
    uint32_t       countele = 0;
    uint64_t       ts       = chaos::common::utility::TimingUtil::getTimeStampInMicroseconds();
    TFile*         fout     = new TFile(dstpath.c_str(), "RECREATE");
    int64_t        oldseq   = -1;
    int64_t        oldrunid = -1;
    int64_t        oldtime  = -1;
    ChaosToTree    ti(srcpath);
    uint32_t       document_len = 0;
    const uint8_t* document     = NULL;
    if (src == NULL) {
      ERR << "cannot open bson for read:" << srcpath;
      fl.unlock();
      return -1;
    }
    while (((b = bson_reader_read(src, &eof)) != NULL) && (eof == false)) {
      bson_iter_t iter;

      if (bson_iter_init(&iter, b) == false) {
        ERR << countele << " invalid iterator";
        countele++;
        continue;
      }
      if (bson_iter_next(&iter) == false) {
        ERR << countele << " invalid next iterator";
        countele++;
        continue;
      }

      bson_iter_document(&iter,
                         &document_len,
                         &document);
      chaos::common::data::CDataWrapper cd((const char*)document, document_len);
      uint64_t                          iseq, irunid;
      int64_t                           tim;

      iseq   = cd.getInt64Value(chaos::DataPackCommonKey::DPCK_SEQ_ID);
      irunid = cd.getInt64Value(chaos::ControlUnitDatapackCommonKey::RUN_ID);
      tim    = cd.getInt64Value(chaos::NodeHealtDefinitionKey::NODE_HEALT_MDS_TIMESTAMP);

      if (oldrunid < 0) {
        oldrunid = irunid;
      }

      if (oldseq < 0) {
        oldseq = iseq;
      } else {
        if ((oldrunid == irunid) && (oldseq + 1) != iseq) {
          ERR << countele << ",s:" << iseq << ",t:" << tim << "] seq discontinuity old:" << oldseq << " new:" << iseq;
          ret++;
        }
      }
      if (oldrunid != irunid) {
        DBG << countele << ",s:" << iseq << ",t:" << tim << "] change in run id, old:" << oldrunid << " new:" << irunid;
        oldrunid = irunid;
      }

      if (oldrunid != irunid) {
        if (oldrunid > irunid) {
          ERR << countele << ",s:" << iseq << ",t:" << tim << "] RUN ID INVERSION!! id, old:" << oldrunid << " new:" << irunid;
          ret++;

        } else {
          DBG << countele << ",s:" << iseq << ",t:" << tim << "] change in run id, old:" << oldrunid << " new:" << irunid;
        }
        oldrunid = irunid;
      }
      if (oldtime > tim) {
        ERR << countele << ",s:" << iseq << ",t:" << tim << "] TIME INVERSION!! id, old:" << oldtime << " new:" << tim;
        oldrunid = irunid;
        ret++;
      }
      oldtime = tim;
      oldseq  = iseq;
      ti.addData(cd);
      countele++;
    }
    fout->Write();

    if (countele > 0) {
      uint64_t tt = chaos::common::utility::TimingUtil::getTimeStampInMicroseconds() - ts;

      DBG << "converted:" << countele << " items in " << 1.0 * tt / 1000 << " ms " << (1.0 * countele * 1000000 / tt) << " item/s into:" << dstpath;
    }
    fout->Close();
  }

  fl.unlock();
  return ret;
}

void GenerateRootJob::processBufferElement(QueueElementShrdPtr element) {
  boost::filesystem::path p(element->c_str());
  std::string             srcpath;
  if (p.extension() == ".lz4") {
    std::string decomp = (*element.get()) + ".uncomp.rootemp";
    if (lz4decomp(*element, decomp) >= 0) {
      createRoot(decomp, *element + ".root");
      boost::filesystem::remove(decomp);
    }
  } else {
    createRoot(*element, *element + ".root");
  }
}
#endif

static int createFinal(const std::string& dstdir, const std::string& name, bool finalize, bool overwrite = false) {
  std::string             dstpath = dstdir + "/" + name+ ((PosixFile::compress)? ".lz4":"");
  boost::filesystem::path p(dstpath);

  FileLock                fl(dstpath + ".lock");
  try {
    fl.lock();
    if (overwrite) {
      DBG << " Overwrite:" + p.string();

      boost::filesystem::remove(p);
    }
    if (!boost::filesystem::exists(p)) {
      std::string              fpath     = dstpath + ".tmp";
      std::vector<std::string> unordered = searchFileExt(dstdir, ".unordered");
      std::vector<std::string> ordered   = searchFileExt(dstdir, ".ordered");
      if (unordered.size() == ordered.size()) {
        // all ordered file were created
        int retw = reorderMulti(fpath, ordered);
        if (retw > 0) {
          if (finalize) {
            #ifdef CERN_ROOT
              if (PosixFile::generateRoot) {
                createRoot(fpath,dstdir );
              }
            #endif

            // create final
            if (PosixFile::compress) {
                lz4compress(fpath, dstpath);
            } else {
              boost::filesystem::rename(fpath, dstpath);
            }

            if (PosixFile::removeTemp) {
                for (std::vector<std::string>::iterator rd = unordered.begin(); rd != unordered.end(); rd++) {
                  DBG << "remove unordered :" << *rd;

                  boost::filesystem::remove(*rd);
                }

                for (std::vector<std::string>::iterator rd = ordered.begin(); rd != ordered.end(); rd++) {
                  DBG << "remove ordered:" << *rd;

                  boost::filesystem::remove(*rd);
                }
            }

            
          } else {
            boost::filesystem::rename(fpath, dstpath);
            DBG << " Create final:" << dstpath << " numobj:" << retw;
          }
          // we can remove all resources bounded to this directory

          return retw;
        }
      }
    } else {
      DBG << " Final:" << dstpath << ", Exist";
      return 0;
    }
  } catch (boost::filesystem::filesystem_error& err) {
    ERR << " FS Error:" << err.what();
  }
  fl.unlock();
  // cannot be still created
  return -1;
}
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
  AsyncCentralManager::getInstance()->addTimer(this, 2000, 2000);
  finalize_th = boost::thread(&PosixFile::finalizeJob, this);
#ifdef CERN_ROOT
  rootGenJob.init(1);
#endif
}
void PosixFile::finalizeJob() {
  DBG << "FINALIZE JOB CREATED";

  exitFinalizeJob = false;
  do {
    boost::mutex::scoped_lock lock(mutex_io);

    wait_data.wait(lock);
    dirpath_t* ele;

    while (file_to_finalize.pop(ele)) {
      DBG << "processing dir :" << ele->dir << " name:" << ele->name;
      if (createFinal(ele->dir, ele->name, true) >= 0) {
      // DBG << " CREATE FINAL: " << fpath;
#ifdef CERN_ROOT
        if (PosixFile::generateRoot) {
          std::string                                                     fpath = ele->dir + "/" + ele->name + ((PosixFile::compress) ? ".lz4" : "");
          chaos::CObjectProcessingQueue<std::string>::QueueElementShrdPtr a(new std::string(fpath));
          rootGenJob.push(a);
        }
#endif

        delete ele;
      }
    }

  } while (!exitFinalizeJob);
  DBG << "FINALIZE JOB EXIT";
}
#define MAX_QUEUE_LEN 20000
PosixFile::~PosixFile() {
  exitFinalizeJob = true;

  AsyncCentralManager::getInstance()->removeTimer(this);
#ifdef CERN_ROOT
  rootGenJob.deinit();
#endif
  wait_data.notify_all();
}
PosixFile::write_path_t PosixFile::s_lastWriteDir;
std::string             PosixFile::serverName;
PosixFile::cacheRead_t  PosixFile::s_lastAccessedDir;
//PosixFile::ordered_t         PosixFile::s_ordered;
PosixFile::searchWorkerMap_t PosixFile::searchWorkers;
//ChaosSharedMutex PosixFile::devio_mutex;
boost::mutex PosixFile::last_access_mutex, PosixFile::cache_mutex;

bool SearchWorker::isExpired() {
  if (done == false) return false;
  const uint64_t now = chaos::common::utility::TimingUtil::getTimeStamp();
  if ((now - lru_ts) > CACHE_EXPIRE_TIME) {
    return true;
  }

  return false;
}
void SearchWorker::pathToCache(const std::string& final_file) {
  try {
    bson_reader_t* reader;
    bson_error_t   src_err;

    bson_iter_t   it;
    const bson_t* bson;
    bson_iter_t   iter;
    bool          end;

    reader = bson_reader_new_from_file(final_file.c_str(), &src_err);
    if (reader == NULL) {
      ERR << " CANNOT OPEN " << final_file;
      wait_data.notify_one();
      elements = 0;
      return;
    }
    int count = 0;
    //elements  = 0;
    //mutex_ele.lock();
    //cache_data.clear();
    // mutex_ele.unlock();
    first_seq = first_runid        = -1;
    uint32_t       elements_stored = elements;
    uint32_t       document_len    = 0;
    const uint8_t* document        = NULL;
    while ((bson = bson_reader_read(reader, &end)) && (end == false)) {
      bson_iter_t iter;
      if (bson_iter_init(&iter, bson) == false) {
        ERR << " iter init false";
        continue;
      }

      if (bson_iter_next(&iter) == false) {
        ERR << " iter next false";
        continue;
      }

      bson_iter_document(&iter,
                         &document_len,
                         &document);
      if (elements_stored == 0) {
        chaos::common::data::CDataWrapper* pd = new chaos::common::data::CDataWrapper((const char*)document, document_len);
        if (pd == NULL) {
          ERR << " invalid bson data, lem:" << document_len << " ptr:0x" << std::hex << document;
          continue;
        }
        // chaos::common::data::CDataWrapper*d=new chaos::common::data::CDataWrapper((const char*)document, document_len);
        uint64_t iseq, irunid, ts;
        iseq                              = pd->getInt64Value(chaos::DataPackCommonKey::DPCK_SEQ_ID);
        irunid                            = pd->getInt64Value(chaos::ControlUnitDatapackCommonKey::RUN_ID);
        ts                                = pd->getInt64Value(NodeHealtDefinitionKey::NODE_HEALT_MDS_TIMESTAMP);
        chaos::common::data::CDWShrdPtr d = chaos::common::data::CDWShrdPtr(pd);

        if (first_seq < 0) {
          first_seq = iseq;
          first_ts  = ts;
        }
        if (first_runid < 0) {
          first_runid = irunid;
        }
        //dont push until we are at the end;
        dataObj_t dob;
        dob.runid = irunid;
        dob.ts    = ts;
        dob.seq   = iseq;
        dob.obj   = d;

        cache_data.push_back(dob);
        elements++;
        last_ts    = ts;
        last_seq   = iseq;
        last_runid = irunid;

        if ((elements % page_len) == 0) {
          wait_data.notify_one();
        }
      }
      if (elements_stored > 0) {
        elements_stored--;
      }
    }
    bson_reader_destroy(reader);

  } catch (std::exception& e) {
    ERR << "Exception occur:" << e.what();
  }
  done = true;
  wait_data.notify_one();
}
std::string SearchWorker::prepareDirectory() {
  std::string             fpath = path + "/" + POSIX_FINAL_DATA_NAME;
  boost::filesystem::path final_file(fpath);
  // check if exists uncompressed
  if (boost::filesystem::exists(final_file)) {
    istemp = false;
    return final_file.string();

  } else if (boost::filesystem::exists(fpath + ".lz4.uncomp") || ((boost::filesystem::exists(fpath + ".lz4") && (lz4decomp((fpath + ".lz4"), (fpath + ".lz4.uncomp")) >= 0)))) {
    return fpath + ".lz4.uncomp";

  } 
  #ifdef MAKE_TEMP_FILES
  else {
    PosixFile::write_path_t::iterator id = PosixFile::s_lastWriteDir.find(path);
    if (id != PosixFile::s_lastWriteDir.end()) {
      boost::filesystem::path  final_file(path + "/" + POSIX_FINAL_DATA_NAME);
      std::string              local_ordered = id->second.fname + POSIX_UNORDERED_EXT;
      std::vector<std::string> ordered;

      if (makeOrdered(id->second) >= 0) {
        int                      retry            = 3;
        int                      incwait          = 1;
        int                      previous_ordered = 0;
        std::vector<std::string> unordered;
        ;
        //creato o gia' creato, check if there are all the ordered
        do {
          previous_ordered = ordered.size();
          unordered        = searchFileExt(id->first, ".unordered");
          ordered          = searchFileExt(id->first, ".ordered");
          if (unordered.size() == 0) {
            ERR << " NO DATA PRESENT in:" << id->first;
            return std::string();
          }
          if (ordered.size() < unordered.size()) {
            DBG << " Not all Ordered created:" << ordered.size() << "/" << unordered.size() << " retry:" << retry;
            usleep(10000 * incwait);
            incwait++;
          }
        } while ((ordered.size() < unordered.size()) && (retry-- > 0));
        if (retry < 0) {
          ERR << " NOT ALL ORDERED PRESENT in:" << id->first << " " << ordered.size() << "/" << unordered.size();
          return std::string();
        }
        bool recreate = (id->second.ordered_ts > id->second.final_ts);
        if (createFinal(path, PosixFile::serverName + "_" + POSIX_TEMPFINAL_DATA_NAME, false, recreate) > 0) {
          id->second.final_ts = chaos::common::utility::TimingUtil::getTimeStamp();
          return (path + "/" + PosixFile::serverName + "_" + POSIX_TEMPFINAL_DATA_NAME);

        } else {
          return std::string();
        }

      } else {
        ERR << " cannot create local ordered in:" << path;
        return std::string();
      }
    }
  }
  #endif
  DBG << " Not yet ready " << fpath << " for:" << path;
  return std::string();
}

void SearchWorker::searchJob(const std::string& dir) {
  done  = false;
  error = 0;
  mutex_job.lock();
  DBG << "START Search searching on:" << dir;
  pathToCache(dir);
  DBG << "END Search Job cache :" << dir << " size:" << cache_data.size() << " elements:" << elements << " ts from:" << chaos::common::utility::TimingUtil::toString(first_ts) << " seq:" << first_seq << " runid:" << first_runid << "  to:" << chaos::common::utility::TimingUtil::toString(last_ts) << " seq:" << last_seq << " runid:" << last_runid;
  mutex_job.unlock();

  // now find all the other files generated by other servers
}

SearchWorker::SearchWorker()
    : max_elements(MAX_QUEUE_LEN) /*,cache_data(MAX_QUEUE_LEN)*/, elements(0) {
  last_seq = last_runid = elements = 0;
  done                             = false;
  istemp                           = true;
  const uint64_t now               = chaos::common::utility::TimingUtil::getTimeStamp();
  lru_ts                           = now;

  DBG << "CREATED SEARCH WORKER " << std::hex << this;
}
int SearchWorker::search(const std::string& p, const uint64_t _timestamp_from, const uint64_t _timestamp_to, uint64_t seq, uint64_t runid, uint32_t maxele) {
  path               = p;
  timestamp_from     = _timestamp_from;
  timestamp_to       = _timestamp_to;
  start_seq          = seq;
  start_runid        = runid;
  page_len           = maxele;
  const uint64_t now = chaos::common::utility::TimingUtil::getTimeStamp();
  lru_ts             = now;
  std::string dir    = prepareDirectory();
  if (dir.size()) {
    DBG << "Starting search thread on path:" << path;

    search_th = boost::thread(&SearchWorker::searchJob, this, dir);
    return 0;
  }
  return -1;
}
// return number of data or negative if error or timeout
bool SearchWorker::waitData(int timeo) {
  if (done) {
    return false;
  }
  boost::mutex::scoped_lock lock(mutex_io);
  boost::system_time const  timeout = boost::get_system_time() + boost::posix_time::milliseconds(timeo);

  //   boost::chrono::system_clock::time_point wakeUpTime =
  //   boost::chrono::system_clock::now() + period;
  DBG << "waiting for data available.." << elements;
  bool ret = wait_data.timed_wait(lock, timeout);
  DBG << "data available:" << elements;

  return ret;
}
//SearchWorker::dataObj::~dataObj(){DBG<<" count :"<<obj.use_count();}

int SearchWorker::getData(abstraction::VectorObject& dst, int maxData, const uint64_t timestamp_from, const uint64_t timestamp_to, chaos::common::direct_io::channel::opcode_headers::SearchSequence& seq, int tim) {
  if (maxData <= 0) {
    ERR << " you should specify a valid data size";
    return 0;
  }

  int cntt = 0;

  DBG << "from:" << chaos::common::utility::TimingUtil::toString(timestamp_from) << " to:" << chaos::common::utility::TimingUtil::toString(timestamp_to) << " runid:" << seq.run_id << " seq:" << seq.datapack_counter;
  uint64_t lseq = 0, lrunid = 0, lts = 0, cntd = 0;
  uint64_t irunid = 0, ts;
  int64_t  iseq   = 0;
  int      index  = 0;
  //for (SafeVector< chaos::common::data::CDataWrapper*>::iterator i = cache_data.begin(); (i != cache_data.end()) && (cntt < maxData); i++) {
  /* for(index=start_index;(index<cache_data.size())&& (index<(start_index+maxData));index++){
        dst.push_back(cache_data[index]);
        cntt++;    
    }*/
  bool again;
  do {
    const uint64_t now = chaos::common::utility::TimingUtil::getTimeStamp();
    lru_ts             = now;

    again = waitData(tim);

    DBG << path << " again:" << again << " index:" << index << " size:" << cache_data.size() << " elements:" << elements << " ts from:" << chaos::common::utility::TimingUtil::toString(first_ts) << " seq:" << first_seq << " runid:" << first_runid << "  to:" << chaos::common::utility::TimingUtil::toString(last_ts) << " seq:" << last_seq << " runid:" << last_runid;
    if ((again == false) && (((last_seq > 0) && (seq.datapack_counter > 0) && (last_seq < seq.datapack_counter)) || ((last_ts > 0) && (timestamp_from > last_ts)))) {
      DBG << " skipping here data not present";
      return 0;
    }
    for (; (index < cache_data.size()) && (cntt < maxData); index++) {
      irunid = cache_data[index].runid;
      if (irunid > seq.run_id) {
        seq.run_id           = irunid;
        seq.datapack_counter = iseq;
      }
      iseq = cache_data[index].seq;
      ts   = cache_data[index].ts;
      if ((ts < timestamp_to) && (ts >= timestamp_from) && (irunid >= seq.run_id) && (iseq >= seq.datapack_counter)) {
        dst.push_back(cache_data[index].obj);
#if CHAOS_PROMETHEUS

        (*PosixFile::counter_read_data_uptr) += cache_data[index].obj->getBSONRawSize();

#endif

        lrunid = irunid;
        lseq   = iseq;
        lts    = ts;
        cntt++;
      }
    }
  } while (again && (cntt < maxData) /*(&& (index < cache_data.size())*/);
  if (cntt > 0) {
    seq.run_id           = lrunid;
    seq.datapack_counter = lseq;
    seq.ts               = lts;
  }

  return cntt;
}
SearchWorker::~SearchWorker() {
  DBG << "DESTROYING elements:" << elements << " path:" << path << " this:" << std::hex << this;

  wait_data.notify_all();
  search_th.join();
  /*cache_data.consume_all([this](chaos::common::data::CDataWrapper* i) {
    delete (i);
    elements--;
  });*/
  /*for (SafeVector<chaos::common::data::CDataWrapper*>::iterator i = cache_data.begin(); (i != cache_data.end()); i++) {
     if(*i){
       delete *i;
     }
   }*/
}

bool existYear(const std::string& prefix, const std::string& cu, const std::string& tag, uint64_t ts_ms) {
  char dir[MAX_PATH_LEN];

  time_t    t = (ts_ms / 1000);
  struct tm tinfo;
  localtime_r(&t, &tinfo);
  if (tag.size()) {
    snprintf(dir, MAX_PATH_LEN, "%s/%s/%s/%.4d", prefix.c_str(), cu.c_str(), tag.c_str(), tinfo.tm_year + 1900);

  } else {
    snprintf(dir, MAX_PATH_LEN, "%s/%s/%.4d", prefix.c_str(), cu.c_str(), tinfo.tm_year + 1900);
  }

  bool exist = boost::filesystem::exists(dir);
  /*if(exist){
    DBG<<" Looking year:"<< tinfo.tm_year + 1900;
  }*/
  return exist;
}

bool existYearMonth(const std::string& prefix, const std::string& cu, const std::string& tag, uint64_t ts_ms) {
  char dir[MAX_PATH_LEN];

  time_t    t = (ts_ms / 1000);
  struct tm tinfo;
  localtime_r(&t, &tinfo);
  if (tag.size()) {
    snprintf(dir, MAX_PATH_LEN, "%s/%s/%s/%.4d/%.2d/", prefix.c_str(), cu.c_str(), tag.c_str(), tinfo.tm_year + 1900, tinfo.tm_mon + 1);

  } else {
    snprintf(dir, MAX_PATH_LEN, "%s/%s/%.4d/%.2d/", prefix.c_str(), cu.c_str(), tinfo.tm_year + 1900, tinfo.tm_mon + 1);
  }

  return boost::filesystem::exists(dir);
}
bool existYearMonthDay(const std::string& prefix, const std::string& cu, const std::string& tag, uint64_t ts_ms) {
  char dir[MAX_PATH_LEN];

  time_t    t = (ts_ms / 1000);
  struct tm tinfo;
  localtime_r(&t, &tinfo);
  if (tag.size()) {
    snprintf(dir, MAX_PATH_LEN, "%s/%s/%s/%.4d/%.2d/%.2d", prefix.c_str(), cu.c_str(), tag.c_str(), tinfo.tm_year + 1900, tinfo.tm_mon + 1, tinfo.tm_mday);
  } else {
    snprintf(dir, MAX_PATH_LEN, "%s/%s/%.4d/%.2d/%.2d", prefix.c_str(), cu.c_str(), tinfo.tm_year + 1900, tinfo.tm_mon + 1, tinfo.tm_mday);
  }

  //DBG<<" Looking existYearMonthDay checking:"<< dir << " date:"<<chaos::common::utility::TimingUtil::toString(ts_ms);

  bool exist = boost::filesystem::exists(dir);

  /* if(exist){
    DBG<<" Looking in:"<<dir;
  }*/
  return exist;
}
void PosixFile::calcFileDir(const std::string& prefix, const std::string& cu, const std::string& tag, uint64_t ts_ms, uint64_t seq, uint64_t runid, char* dir, char* fname) {
  // std::size_t found = cu.find_last_of("/");
  //time_t     t     = (ts_ms==0)?time(NULL):(ts_ms / 1000);
  time_t    t = (ts_ms / 1000);
  struct tm tinfo;
  localtime_r(&t, &tinfo);

  // CU PATH NAME/<yyyy>/<mm>/<dd>/<hour>
  if (tag.size() > 0) {
    snprintf(dir, MAX_PATH_LEN, "%s/%s/%s/%.4d/%.2d/%.2d/%.2d/%.2d", prefix.c_str(), cu.c_str(), tag.c_str(), tinfo.tm_year + 1900, tinfo.tm_mon + 1, tinfo.tm_mday, tinfo.tm_hour, tinfo.tm_min);

  } else {
    snprintf(dir, MAX_PATH_LEN, "%s/%s/%.4d/%.2d/%.2d/%.2d/%.2d", prefix.c_str(), cu.c_str(), tinfo.tm_year + 1900, tinfo.tm_mon + 1, tinfo.tm_mday, tinfo.tm_hour, tinfo.tm_min);
  }
  // timestamp_runid_seq_ssss
  snprintf(fname, MAX_PATH_LEN, "%lu_%010lu_%010lu", ts_ms, runid, seq);
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
  const uint64_t now = chaos::common::utility::TimingUtil::getTimeStamp();

  const uint64_t ts = stored_object.getInt64Value(NodeHealtDefinitionKey::NODE_HEALT_MDS_TIMESTAMP);
  char           dir[MAX_PATH_LEN];
  char           f[MAX_PATH_LEN];
  uint8_t*       buf;
  size_t         buflen;
  int64_t        seq, runid;
  std::string    tag;
  try {
    if ((meta_tags.get())&& (meta_tags->size() > 0)) {
      //tag=std::accumulate(meta_tags->begin(),meta_tags->end(),std::string("_"));
      tag = boost::algorithm::join(*meta_tags.get(), "_");
    }
    seq   = stored_object.getInt64Value(chaos::DataPackCommonKey::DPCK_SEQ_ID);
    runid = stored_object.getInt64Value(chaos::ControlUnitDatapackCommonKey::RUN_ID);
    calcFileDir(basedatapath, key, tag, ts, seq, runid, dir, f);
    bool                   notexist;
    write_path_t::iterator id;
    {
      boost::mutex::scoped_lock lk(last_access_mutex);
      id       = s_lastWriteDir.find(dir);
      notexist = (id == s_lastWriteDir.end());
      //last_access_mutex.unlock();
      //ChaosWriteLock ll(id->second.devio_mutex);
      if (notexist /*|| (ts - (id->second).ts) > 1000*/) {
        boost::filesystem::path p(dir);
        if ((boost::filesystem::exists(p) == false)) {
          try {
            if ((boost::filesystem::create_directories(p) == false) && ((boost::filesystem::exists(p) == false))) {
              ERR << "cannot create directory:" << p << " runid:" << runid << " seq:" << seq;
              // last_access_mutex.unlock();

              //  return -1;
            } else {
              DBG << " CREATED DIR:" << p;
            }
          } catch (boost::filesystem::filesystem_error& e) {
            ERR << " Exception creating directory:" << e.what();
          }
          //
        }

        std::string    path = std::string(dir) + "/" + serverName + POSIX_UNORDERED_EXT;
        ChaosWriteLock ll(s_lastWriteDir[dir].devio_mutex);
        s_lastWriteDir[dir].fname = path;
        s_lastWriteDir[dir].ts    = now;
        if (s_lastWriteDir[dir].writer.open(path) != 0) {
          ERR << "cannot open "
              << " mapped file:" << path;
          // last_access_mutex.unlock();
          s_lastWriteDir.erase(dir);
          return -2;
        }
        id = s_lastWriteDir.find(dir);
      }
    }
    ChaosWriteLock ll(id->second.devio_mutex);

    //BsonFStream& writer = s_lastWriteDir[dir].writer;
    bool ok;
    int  retry = 2;
    do {
      char key[32];
      snprintf(key, sizeof(key), "%010lu_%010lu", runid, seq);

      if ((ok = (id->second.writer.write(key, stored_object) > 0))) {
        id->second.last_seq   = seq;
        id->second.last_runid = runid;
        id->second.keys.push_back(key);

#if CHAOS_PROMETHEUS

        (*counter_write_data_uptr) += stored_object.getBSONRawSize();

#endif
        id->second.unordered_ts = now;
      } else {
        ERR << " CANNOT WRITE:" << dir << " seq:" << seq << " runid:" << runid << " retry:" << retry;

        boost::filesystem::path p(dir);
        if ((boost::filesystem::exists(p) == false)) {
          try {
            if ((boost::filesystem::create_directories(p) == false) && ((boost::filesystem::exists(p) == false))) {
              ERR << "cannot create directory:" << p << " runid:" << runid << " seq:" << seq;
              // last_access_mutex.unlock();

              //  return -1;
            } else {
              DBG << " CREATED DIR:" << p;
            }
          } catch (boost::filesystem::filesystem_error& e) {
            ERR << " Exception creating directory:" << e.what();
          }
        }
        //
      }
    } while ((ok == false) && (retry--));

#if CHAOS_PROMETHEUS

    (*gauge_insert_time_uptr) = (chaos::common::utility::TimingUtil::getTimeStamp() - ts);
#endif
    return (ok == false) ? -2 : 0;
  } catch (const boost::exception& e) {
    ERR << "boost exception :" << diagnostic_information(e);
    return -400;
    // display your error message here, then do whatever you need to, e.g.
  } catch (std::exception const& ex) {
    ERR << "std exception :" << ex.what();
    return -401;

  } catch (...) {
    ERR << "uknown exception";
    return -402;
  }
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
  uintmax_t ret;
  if ((ret=remove_all(p)>=0)) {
      DBG << "REMOVED " << p << " items:"<<ret;
      if (boost::filesystem::is_empty(p.parent_path())) {
          return removeRecursevelyUp(p.parent_path());
      }
  }
  
  return 0;
}

 int PosixFile::removeMinutes(const std::string& key,uint64_t start_ts,uint64_t end_ts){
  int removed=0;
  char     dir[MAX_PATH_LEN];
  char     f[MAX_PATH_LEN];

for (uint64_t start = start_ts; start < end_ts; ) {
    calcFileDir(basedatapath, key, "", start, 0, 0, dir, f);
    boost::filesystem::path p(dir);
     DBG << "LOOKING IN " << p <<" parent:"<<p.parent_path();
    try{
    if (boost::filesystem::exists(p)) {
      removed+=removeRecursevelyUp(p);
    } else if ((!boost::filesystem::exists(p.parent_path()))||(boost::filesystem::is_empty(p.parent_path()))) {

        //no other minutes in this hour
        start+=(POSIX_HOURS_MS-(start%POSIX_HOURS_MS)); // jump next hour
   //     DBG << "PARENT HOUR EMPTY " << p.parent_path() << " incrementing of:"<<(POSIX_HOURS_MS-(start%POSIX_HOURS_MS));

        removed+=removeRecursevelyUp(p.parent_path());

        if((!boost::filesystem::exists(p.parent_path().parent_path()))||(boost::filesystem::is_empty(p.parent_path().parent_path()))){
          
          // no other hours in day
          start+=(POSIX_DAY_MS-(start%POSIX_DAY_MS)); // jump next day
    //      DBG << "PARENT DAY EMPTY " << p.parent_path() << " incrementing of:"<<(POSIX_DAY_MS-(start%POSIX_DAY_MS));

        }
      continue;
    }
    } catch (boost::filesystem::filesystem_error& e) {
            ERR << " Exception creating directory:" << e.what();
    } catch(...){
      ERR<<" error accessing:"<<p;

    }
    start += (POSIX_MINUTES_MS);

  }
  return removed;
}

//!delete objects that are contained between intervall (exstreme included)
int PosixFile::deleteObject(const std::string& key,
                            uint64_t           start_timestamp,
                            uint64_t           end_timestamp) {
  uint64_t start_aligned = start_timestamp - (start_timestamp % (3600 * 1000));
  char     dir[MAX_PATH_LEN];
  char     f[MAX_PATH_LEN];

  
  DBG << "Searching \"" << key << "\" from: " << chaos::common::utility::TimingUtil::toString(start_timestamp) << " to:" << chaos::common::utility::TimingUtil::toString(end_timestamp);
  boost::filesystem::path p(basedatapath+"/"+key);
  if (!boost::filesystem::exists(p)|| !is_directory(p)) {
      DBG << "Path \"" << p << "\" does not exist any more"; 

      return 0;
  } 
  if(end_timestamp<start_timestamp){
    ERR<<" Time specification start mustbe smaller than end";
    return -1;
  }

  
  
  if(start_timestamp==0 && (end_timestamp>=(chaos::common::utility::TimingUtil::getTimeStamp()-1000))){
    //remove directly top directory
      DBG << " FAST REMOVING " << p;
      uintmax_t ret;
      if ((ret=remove_all(p)) >= 0) {
          DBG << " REMOVED :" << ret << " items";

        //removeRecursevelyUp(p);
      } 
      return 0;
  }
  
  
  int ret=removeMinutes(key,start_timestamp,end_timestamp);
  DBG << "REMOVING ret: " << ret;

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
      sscanf(it->c_str(), "%lu_%lu_%lu", &tim, &irunid, &iseq);
      tim *= 1000;
      if ((tim < timestamp_to) && (tim >= timestamp_from)) {
        elements++;
      }
    }
  }
  return elements;
}

int PosixFile::getFromPath(const std::string& dir, const uint64_t timestamp_from, const uint64_t timestamp_to, const uint32_t page_len, abstraction::VectorObject& found_object_page, chaos::common::direct_io::channel::opcode_headers::SearchSequence& last_record_found_seq) {
  //   const cacheRead_t::iterator di=s_lastAccessedDir.find(dir);
  //  if(di==s_lastAccessedDir.end()){
  cache_mutex.lock();
  searchWorkerMap_t::iterator i = searchWorkers.find(dir);
  cache_mutex.unlock();
  if (i == searchWorkers.end()) {
    if (searchWorkers[dir].search(dir, timestamp_from, timestamp_to, 0, 0) == 0) {
      int ret = searchWorkers[dir].getData(found_object_page, page_len, timestamp_from, timestamp_to, last_record_found_seq);
      DBG << "1- RETURNED:" << ret << "/" << i->second.elements << " buf size:" << found_object_page.size() << " last runid:" << last_record_found_seq.run_id << " last seq:" << last_record_found_seq.datapack_counter;

      return ret;
    } else {
      
      searchWorkers.erase(dir);
      return 0;
    }
  } else {
    // recalculate if not final
    if (i->second.isTemp()) {
      if (i->second.search(dir, timestamp_from, timestamp_to, 0, 0) == 0) {
        int ret = i->second.getData(found_object_page, page_len, timestamp_from, timestamp_to, last_record_found_seq);
        DBG << "RETURNED:" << ret << "/" << i->second.elements << " buf size:" << found_object_page.size() << " last runid:" << last_record_found_seq.run_id << " last seq:" << last_record_found_seq.datapack_counter;
        return ret;
      } else {
        ERR << " Something wrong in search again.. removing";
        searchWorkers.erase(dir);
        return -1;
      }
    }
    int ret = i->second.getData(found_object_page, page_len, timestamp_from, timestamp_to, last_record_found_seq);
    DBG << "RETURNED2:" << ret << "/" << i->second.elements << " buf size:" << found_object_page.size() << " last runid:" << last_record_found_seq.run_id << " last seq:" << last_record_found_seq.datapack_counter;
    return ret;

    // int ret = i->second.getData(found_object_page, page_len, timestamp_from,timestamp_to,last_record_found_seq.run_id, last_record_found_seq.datapack_counter);
  }
  return 0;
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

    if ((meta_tags.size() > 0)) {
      //tag=std::accumulate(meta_tags.begin(),meta_tags.end(),std::string("_"));
      tag = boost::algorithm::join(meta_tags, "_");
    }
    DBG << "Search " << key << " from: " << timestamp_from << "[" << chaos::common::utility::TimingUtil::toString(timestamp_from) << "] to:" << timestamp_to << "[" << chaos::common::utility::TimingUtil::toString(timestamp_to) << "] tags:" << tag << " seqid:" << seqid << " runid:" << runid;

    // align to minute
    uint64_t start_aligned = timestamp_from - (timestamp_from % (60 * 1000));
    uint64_t stop_aligned  = timestamp_to;  // + ((60 * 1000));

    // loop years
    // loop months
    // loop days
    // loop houes
    //time_t start_s = (timestamp_from / 1000);
    //time_t end_s   = (timestamp_to / 1000);

    for (uint64_t years_timestamp = start_aligned; years_timestamp < timestamp_to;) {
      struct tm info;
      time_t    start_s = years_timestamp / 1000;
      localtime_r(&start_s, &info);

      if (existYear(basedatapath, key, tag, years_timestamp)) {
        for (uint64_t day_timestamp = years_timestamp; day_timestamp < timestamp_to; day_timestamp += POSIX_DAY_MS) {
          if (existYearMonthDay(basedatapath, key, tag, day_timestamp)) {
            // Since I cut precision to 1 sec I've to look 1 sec more,
            for (uint64_t start = day_timestamp; start < (stop_aligned); start += POSIX_MINUTES_MS) {
              //    time_t t = (start / 1000);
              //struct tm* tinfo = localtime(&t);
              //      struct tm tinfo;
              //       localtime_r(&t, &tinfo);
              //    if ((tinfo.tm_min != old_hour)) {
              calcFileDir(basedatapath, key, tag, start, seqid, runid, dir, f);
              // boost::filesystem::path p(dir);
              if (!boost::filesystem::exists(dir)) {
                //   DBG << "[" << chaos::common::utility::TimingUtil::toString(start) << "] Looking in \"" << dir << "\" seq:" << seqid << " runid:" << runid << " NOT EXISTS";
                continue;
              }
              DBG << "[" << chaos::common::utility::TimingUtil::toString(start) << "-" << chaos::common::utility::TimingUtil::toString(timestamp_to) << "->" << chaos::common::utility::TimingUtil::toString(stop_aligned) << " ] Looking in \"" << dir << "\" seq:" << seqid << " runid:" << runid;

              elements += getFromPath(dir, timestamp_from, timestamp_to, (page_len - elements), found_object_page, last_record_found_seq);
              if (elements >= page_len) {
                DBG << "[" << dir << "] FOUND " << elements << " page:" << page_len << " last runid:" << last_record_found_seq.run_id << " last seq:" << last_record_found_seq.datapack_counter << " last ts:" << last_record_found_seq.ts;
#if CHAOS_PROMETHEUS

                (*gauge_query_time_uptr) = (chaos::common::utility::TimingUtil::getTimeStamp() - ts);
#endif
                return 0;
              } else if (elements == 0) {
                DBG << "[" << dir << "] NO ELEMENTS FOUND last runid:" << last_record_found_seq.run_id << " last seq:" << last_record_found_seq.datapack_counter << " last ts:" << last_record_found_seq.ts;
              }
              //   old_hour = tinfo.tm_min;
              //   }
            }
          }
        }
      }
      if (((info.tm_year + 1900) % 4) == 0) {
        years_timestamp += POSIX_YEARB_MS;
      } else {
        years_timestamp += POSIX_YEAR_MS;
      }
    }
  } catch (const chaos::CException& e) {
    ERR << "Chaos Exception :" << e.errorMessage;
  } catch (const std::exception& e) {
    ERR << " StdException :" << e.what();
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

void PosixFile::timeout() {

  // remove directory write cache
  for (write_path_t::iterator id = s_lastWriteDir.begin(); id != s_lastWriteDir.end(); ) {
    uint64_t ts = chaos::common::utility::TimingUtil::getTimeStamp();

    if ((ts - id->second.ts) > (POSIX_MSEC_QUANTUM)) {
      //not anymore used
      /* if (last_access_mutex.try_lock() == false) {
        continue;
      }*/
      ChaosWriteLock(id->second.devio_mutex);

      // last_access_mutex.unlock();

      std::string dstdir = id->first;

      std::string fname=dstdir + "/" + POSIX_FINAL_DATA_NAME + ((PosixFile::compress)? ".lz4":"");

      if (boost::filesystem::exists(fname)) {
        // someone else created and cleaned
        id->second.writer.close();
        DBG << dstdir << " CLOSE :"<<dstdir<<" OBJS:"<<id->second.writer.nobj();

        s_lastWriteDir.erase(id++);

        id++;
        continue;
      }
      int ret;
      if ((ret = makeOrdered(id->second)) > 0) {
        dirpath_t* ele = new dirpath_t();
        ele->dir       = dstdir;
        ele->name      = POSIX_FINAL_DATA_NAME;
        DBG << "TO Process" << dstdir;

        file_to_finalize.push(ele);
        wait_data.notify_all();

        /*if (createFinal(dstdir, POSIX_FINAL_DATA_NAME) >= 0) {

          DBG << " CREATE FINAL: " <<dstdir + "/"+POSIX_FINAL_DATA_NAME;
          
        }*/
      } else if (ret < 0) {
        DBG << "remove resource:" << dstdir;

        s_lastWriteDir.erase(id++);

        id++;
        continue;
      }
    }
    id++;
  }

  for (searchWorkerMap_t::iterator i = searchWorkers.begin(); i != searchWorkers.end();) {
    if (i->second.isExpired()) {
      if (cache_mutex.try_lock()) {
        DBG << "removing cache data:" << i->first;

        searchWorkers.erase(i++);
        cache_mutex.unlock();
        continue;
      }
    }
    i++;
  }

  /* for (cacheRead_t::iterator id = s_lastAccessedDir.begin(); id != s_lastAccessedDir.end(); id) {
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
  }*/
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
