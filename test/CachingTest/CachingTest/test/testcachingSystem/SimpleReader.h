//
//  MaxResReader.h
//  CachingSystem
//
//  Created by Flaminio Antonucci on 19/11/12.
//
//

#ifndef CachingSystem_MaxResReader_h
#define CachingSystem_MaxResReader_h

#include <sys/time.h>
#include <boost/thread.hpp>
#include <boost/date_time.hpp>
#include <fstream>
#include <chaos/common/caching_system/common_buffer/CommonBuffer.h>
#include "MioElemento.h"
#include <chaos/common/caching_system/common_buffer/IteratorReader.h>
#include <chaos/common/caching_system/common_buffer/SmartPointer.h>
#include <iostream>
using namespace std;
class SimpleDataReader {
    
private:
    IteratorReader<Magnete>* iteratore;
    bool interrupted;
    long fps;
    double timeInterval;
    std::string basePath;
    int id;
    caching_system::caching_thread::CombinedTrackers<Magnete>* deviceTracker;
    
public:
    SimpleDataReader(IteratorReader<Magnete>* it, long fps,    std::string basePath,int id ,caching_system::caching_thread::CombinedTrackers<Magnete>* deviceTracker){
        
        this->iteratore=it;
        interrupted=false;
        this->fps=fps;
        this->basePath=basePath;
        this->id=id;
        timeInterval= (1/((double) fps))*1000;
        this->deviceTracker=deviceTracker;
       
        
    }
    
    void operator()(){
        //std::cout <<timeInterval;
        clock_t t1, t2;
        boost::posix_time::milliseconds waitingData(timeInterval);

        std::ofstream output;
        std::stringstream path;
        std::vector<SmartPointer<Magnete >* >* buffer= new std::vector<SmartPointer<Magnete> * >();
        
        
        
        path<<basePath<<"reader_"<<id;
        output.open(path.str().c_str());
        time_t inizio =time(0);
        
        
        output << "lettore di dati\n" << std::endl;
        output<<"start time: "<<inizio<<"\n";
        
        while(!interrupted){
            
            std::stringstream ss;
            t1 = clock();
            int count=iteratore->bufferedReader(-1,buffer );
            t2 = clock();
           // std::cout<<"count: "<<count;

            if(count==0){
                boost::this_thread::sleep(waitingData);
                sched_yield();
               // cout<<"non ho nulla da leggere\n";
                
            }else{
            //  cout<<"ho da leggere\n";

                for(int i =0; i<buffer->size();i++){
                    SmartPointer<Magnete >* temp=buffer->at(i);
                    
                    
                    ss<< id << " : "<<(*(temp->operator->()));
                    ss<<"\n";
                    delete temp;
                }
                buffer->clear();
                boost::this_thread::sleep(waitingData);
                sched_yield();
                
                
            }
            
            output<<ss.str();
           // output.flush();
        }
        
        //delete iteratore;
        
        this->deviceTracker->closeBuffer(iteratore);
        
        time_t fine =time(0);
        
        output<<"end time: "<<fine<<"\n";
        
        output << "Chiuso il lettore: finished\n" << std::endl;

        output.close();

        
    }
    
    
            
    void interrupt(){
        this->interrupted=true;
    }
            
            
        
            
};

#endif
