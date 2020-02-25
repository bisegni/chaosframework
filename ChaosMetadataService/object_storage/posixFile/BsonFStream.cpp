#include "BsonFStream.h"
static void*
test_bson_writer_custom_realloc_helper(void* mem, size_t num_bytes, void* ctx) {
  boost::iostreams::mapped_file* mf = (boost::iostreams::mapped_file*)ctx;
  mf->resize(2 * num_bytes);

  return mf->data();
}
BsonFStream::BsonFStream():writer(NULL),objs(0),fsize(0){

}

BsonFStream::BsonFStream(const std::string&fname,int inital_size):writer(NULL),objs(0),fsize(0){
  if(open(fname,inital_size)==0){
    name=fname;
  }
}
BsonFStream::~BsonFStream(){
  close();
}
int BsonFStream::open(const std::string&fname,int size){
        boost::iostreams::mapped_file_params params;
        params.path          = fname;
        params.new_file_size = size;
        params.flags         = boost::iostreams::mapped_file::mapmode::readwrite;
        if(writer){
          return -3;
        }
        mf.open(params);
        if(mf.is_open()){
          
          buf                        = (uint8_t*)mf.data();
          buflen                     = params.new_file_size;
          name=fname;
          writer = bson_writer_new(&buf, &buflen, 0, test_bson_writer_custom_realloc_helper, (void*)&mf);
          if(writer==NULL){
            return -4;
          }
          return 0;
        }
        return -1;
}
    size_t BsonFStream::size(){
        if(writer)
          return bson_writer_get_length(writer);
        return fsize;

    }
    uint32_t BsonFStream::nobj(){
      return objs;
    }

    int BsonFStream::close(){
      if(writer){
        fsize=bson_writer_get_length(writer);
        mf.resize(fsize);
        bson_writer_destroy(writer);
      }
      writer=NULL;
      mf.close();
      return 0;
    }
    int BsonFStream::write(const std::string&key,const chaos::common::data::CDataWrapper&ptr){
          bson_t* b = NULL;
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
          if (bson_writer_begin(writer, &b)) {
             bson_append_value(b, key.c_str(), -1, ptr);
             objs++;
             bson_writer_end(writer);
            return objs;
          }
      return 0;
    }
