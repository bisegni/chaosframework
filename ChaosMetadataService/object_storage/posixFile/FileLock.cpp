#include "FileLock.h"
#include <boost/interprocess/sync/file_lock.hpp>
#include <boost/filesystem.hpp>
#include <chaos/common/configuration/GlobalConfiguration.h>

#define INFO INFO_LOG(BsonFStream)
#define DBG DBG_LOG(BsonFStream)
#define ERR ERR_LOG(BsonFStream)

FileLock::FileLock(const std::string& _name): name(_name) {
    locked = 0;
    f.open(name, std::ios_base::app);
  }
FileLock::~FileLock() {
    if (locked) {
      unlock();
    }
    boost::filesystem::remove(name);
  }
void FileLock::lock() {
    if (locked == 0) {
     std::lock_guard<std::mutex> lock(devio_mutex);
      try {
        boost::interprocess::file_lock f(name.c_str());
        locked++;

        f.lock();
      } catch (boost::interprocess::interprocess_exception& e) {
        ERR << " cannot create lock:" << name.c_str() << " error:" << e.what();
      }
    }
  }
  void FileLock::unlock() {
    if (locked) {
      try {
        boost::interprocess::file_lock f(name.c_str());
        f.unlock();
        locked--;

      } catch (boost::interprocess::interprocess_exception& e) {
        ERR << " cannot unlock:" << name.c_str() << " error:" << e.what();
      }
      devio_mutex.unlock();
    }
  }
  bool FileLock::trylock() {
    if (devio_mutex.try_lock()) {
      boost::interprocess::file_lock f(name.c_str());
      locked++;

      return f.try_lock();
    }
    return false;
  }
