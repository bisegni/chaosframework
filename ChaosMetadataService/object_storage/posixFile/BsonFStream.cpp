#include "BsonFStream.h"
#include <chaos/common/configuration/GlobalConfiguration.h>
#include <chaos/common/utility/TimingUtil.h>

#define INFO INFO_LOG(BsonFStream)
#define DBG DBG_LOG(BsonFStream)
#define ERR ERR_LOG(BsonFStream)
static void*
test_bson_writer_custom_realloc_helper(void* mem, size_t num_bytes, void* ctx) {
  boost::iostreams::mapped_file* mf = (boost::iostreams::mapped_file*)ctx;
  mf->resize(num_bytes);

  return mf->data();
}
BsonFStream::BsonFStream():writer(NULL),objs(0),fsize(0),open_ts(0),close_ts(0){

}

BsonFStream::BsonFStream(const std::string&fname,int inital_size):writer(NULL),objs(0),fsize(0){
  if(open(fname,inital_size)==0){
    name=fname;
  }
}
BsonFStream::~BsonFStream(){
  //DBG<<" DESTROY:"<<name;
  close();
}
int BsonFStream::open(const std::string&fname,int size){
        std::lock_guard<std::mutex> lock(wmutex);

        boost::iostreams::mapped_file_params params;
        params.path          = fname;
        params.new_file_size = size;
        params.flags         = boost::iostreams::mapped_file::mapmode::readwrite;
        name=fname;
        if(writer){
          ERR<<" already open:"<<name;
          return 0;
        }
      //  DBG<<"opening:"<<name<<" x"<<std::hex<<this;
        mf.open(params);
        if(mf.is_open()){
          
          buf                        = (uint8_t*)mf.data();
          buflen                     = params.new_file_size;
          name=fname;
          writer = bson_writer_new(&buf, &buflen, 0, test_bson_writer_custom_realloc_helper, (void*)&mf);
          if(writer==NULL){
            ERR<<" cannot open a new writer for:"<<name;
            return -4;
          }
          open_ts=chaos::common::utility::TimingUtil::getTimeStamp();
          return 0;
        }
        return -1;
}
    size_t BsonFStream::size(){
        std::lock_guard<std::mutex> lock(wmutex);

        if(writer)
          return bson_writer_get_length(writer);
        return fsize;

    }
    uint32_t BsonFStream::nobj(){
      return objs;
    }

    int BsonFStream::close(){
       // DBG<<" Closing:"<<name;
      std::lock_guard<std::mutex> lock(wmutex);
      if(writer){
        fsize=bson_writer_get_length(writer);
        mf.resize(fsize);
        bson_writer_destroy(writer);
      }
      close_ts=chaos::common::utility::TimingUtil::getTimeStamp();

      writer=NULL;
      mf.close();
      return 0;
    }
    int BsonFStream::write(const std::string&key,const chaos::common::data::CDataWrapper&ptr){
          bson_t* b = NULL;
        std::lock_guard<std::mutex> lock(wmutex);

          if(writer==NULL){
            ERR<<"attempt to write not open or closes:"<<name<< " open time:"<<chaos::common::utility::TimingUtil::toString(open_ts)<<" close time:"<<chaos::common::utility::TimingUtil::toString(close_ts)<<" live:"<<(close_ts-open_ts)<<" ms"<< std::hex<<" x"<<this;
            return 0;
          }
          if (bson_writer_begin(writer, &b)) {
             bson_append_document(b, key.c_str(), -1, ptr.getBSON());
             objs++;
             bson_writer_end(writer);
            return objs;
          }
      return 0;
    }
    int BsonFStream::write(const std::string&key,const bson_value_t*ptr){
        bson_t* b = NULL;
          std::lock_guard<std::mutex> lock(wmutex);

           if(writer==NULL){
            ERR<<"attempt to write after close:"<<name<< " open time:"<<chaos::common::utility::TimingUtil::toString(open_ts)<<" close time:"<<chaos::common::utility::TimingUtil::toString(close_ts)<<" live:"<<(close_ts-open_ts)<<" ms";
            ;
            return 0;
          }

          if (bson_writer_begin(writer, &b)) {
             bson_append_value(b, key.c_str(), -1, ptr);
             objs++;
             bson_writer_end(writer);
            return objs;
          }
      return 0;
    }
