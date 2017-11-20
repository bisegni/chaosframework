#include <chaos/common/data/CDataWrapper.h>

int main(){
    const char* test_json_translation="{\"double_key\":[1.0,2.1,-1.0,-0.9]}";
    double test_var[]={1.0,2.1,-1.0,-0.9};
    double u[10];
    chaos::common::data::CDataWrapper data;
    data.setSerializedJsonData(test_json_translation);

    if(data.hasKey("double_key")&&data.isVectorValue("double_key")){

        chaos::common::data::CMultiTypeDataArrayWrapper* p=data.getVectorValue("double_key");
        if(p==NULL){
            std::cerr<<" ## bad return "<<std::endl;
            return -2;
        }
        for(int cnt=0;cnt<p->size();cnt++){
            u[cnt] = p->getDoubleElementAtIndex(cnt);
            std::cout<< "COEFF u["<<cnt<<"]="<<u[cnt]<<std::endl;
            if(u[cnt] != test_var[cnt]){
                std::cerr<<"Error mismatched values at ["<<cnt<<"]"<< " expected:"<<test_var[cnt]<<" read:"<<u[cnt]<<std::endl;
                return -2;
            }

        }
    } else {
        std::cerr<<" ## cannot find key or vector type"<<std::endl;
        return -1;
    }
    return 0;
}
