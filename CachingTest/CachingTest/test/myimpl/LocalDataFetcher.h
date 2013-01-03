//
//  LocalDataFetcher.h
//  CachingSystem
//
//  Created by Flaminio Antonucci on 19/11/12.
//
//

#ifndef CachingSystem_LocalDataFetcher_h
#define CachingSystem_LocalDataFetcher_h


#include "MioElemento.h"
#include <chaos/common/caching_system/caching_thread/tracker_interface/DataFetcherInterface.h>
#include <chaos/common/caching_system/common_buffer/helper/MillisTime.h>
#include <chaos/common/caching_system/common_buffer/IteratorReader.h>
#include <boost/thread.hpp>
#include <boost/date_time.hpp>
#include <vector>
#include <chaos/common/caching_system/common_buffer/SmartPointer.h>






class LocalDataFetcher : public caching_system::DataFetcherInterface<Magnete> {
    
private:
    std::vector<double*> lettori ;

    long waitForData;//millis
    IteratorReader<Magnete>* it;
    std::vector< SmartPointer<Magnete>* >* vettoreDati;
    
public:
    
    LocalDataFetcher(IteratorReader<Magnete>* it){
        this->it=it;
        waitForData=20;
        vettoreDati=new  std::vector<SmartPointer<Magnete>* >();
    
    }
    
    
    //restituisce il valore piu recente da una coda gia cached
    void getData(Magnete *newData, uint64_t* ts){
        
        boost::posix_time::milliseconds waitingData(waitForData);

       
        int count=0;
        
        while(true){
            count=it->bufferedReader(-1,vettoreDati );
            
            if(count==0){
                boost::this_thread::sleep(waitingData);

                sched_yield();
            }else{
                break;
            }
        
        }
        
        Magnete* ritorno=vettoreDati->back()->getData();
        
        //copia i dati
        *newData=*ritorno;
        *ts = my_time::getMillisTimestamp();

        //cancella tutto quello che non vuoi
        
        for(int j =0; j<vettoreDati->size();j++){
            
            //tieni solo l'ultimo
            SmartPointer<Magnete >* temp=vettoreDati->at(j);
            
            
            delete temp;
        }
        vettoreDati->clear();


        
    }
};

#endif
