#ifndef __FILE_LOCK_H__
#define __FILE_LOCK_H__
#include <fstream>
#include <mutex>

class FileLock {
  std::ofstream     f;
  const std::string name;
  std::mutex devio_mutex;
  int               locked;

 public:
  FileLock(const std::string& _name);
     
  ~FileLock();
  void lock();
  void unlock();
  bool trylock();
  std::string getName(){return name;}
};
#endif