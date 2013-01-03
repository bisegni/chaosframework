//
//  main.cpp
//  CachingSystem
//
//  Created by Flaminio Antonucci on 26/07/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//
#include <iostream>
#include <boost/thread.hpp>
#include <boost/date_time.hpp>
#include <map>
#include <sys/stat.h>

//trackers
#include <chaos/common/caching_system/caching_thread/trackers/TransformDeviceTracker.h>
#include <chaos/common/caching_system/caching_thread/trackers/AgregateDeviceTracker.h>


#include <chaos/common/caching_system/caching_thread/trackers/ConcreteDeviceTracker.h>

//implementazione interfacce
#include "SimpleDataFetcher.h"
#include "SimpleDataTransform.h"
#include "SimpleDataAggregator.h"
#include "LocalDataFetcher.h"
#include "SimpleReader.h"
#include "AggregateReader.h"

//strutture dati
#include <chaos/common/caching_system/common_buffer/CommonBuffer.h>
#include "MioElemento.h"
#include "DataAggregate.h"
#include <chaos/common/caching_system/common_buffer/helper/MillisTime.h>



using namespace boost::interprocess;
using namespace caching_system::caching_thread;
using namespace caching_system;

using namespace std;

string basePath="./log";
int tempoDiLavoro=7200;

bool processInput(int argc, const char * argv[]);
void printHelp();


//questo test crea 4 lettori in alta risoluzione, 4 in bassa risoluzione, un sub lettore , un tracker che crea un dato
//aggregato e 4 lettori che leggono il dato aggragato
int run(int argc, const char * argv[]);

int main(int argc, const char * argv[]){
    
    
    run(argc,argv);
}




bool processInput(int argc, const char * argv[]){
    map<string, int> argomenti;
    map<string, int> argomentiConosciuti;
    argomentiConosciuti["-hzWrite"]=1;
    argomentiConosciuti["-fps"]=1;
    argomentiConosciuti["-codeHighRes"]=1;
    argomentiConosciuti["-threads"]=1;
    argomentiConosciuti["-cache"]=1;
    argomentiConosciuti["-worktime"]=1;
    argomentiConosciuti["-validity"]=1;
    argomentiConosciuti["-help"]=1;
    
    if((argc%2==0||((argc-1)/2)>(argomentiConosciuti.size()-1))){
        cout<<"numero di argomenti errato\n";
        printHelp();
        return false;
        
    }
    
    for(int i =1; i<argc;i=i+2){
        
        //cout<<"Argomento "<<i<<": "<<argv[i]<<"\n";
        argomenti[argv[i]]=atoi(argv[i+1]);
        if(argomentiConosciuti[argv[i]]!=1){
            cout<<argv[i]<<" : Argomento sconosciuto. \n";
            printHelp();
            return false;
        }
        
    }
    
    if(argomenti["-hzWrite"]!=0){
        //   timeToFetchData=1000*((((double)1)/((double) argomenti["-hzWrite"])));
        
    }
    if(argomenti["-fps"]!=0){
        //   timeToReadData=1000*((((double)1)/((double) argomenti["-fps"])));
        
    }
    if(argomenti["-threads"]!=0){
        //   threadNumbers=argomenti["-threads"];
    }
    if(argomenti["-worktime"]!=0){
        //    tempoDiLavoro=argomenti["-worktime"];
    }
    if(argomenti["-validity"]!=0){
        //    millisecondiDiValidita=argomenti["-validity"];
    }
    if(argomenti["-cache"]!=0){
        //    cacheMultiplier=argomenti["-cache"];
    }
    if(argomenti["-codeHighRes"]!=0){
        //   numeroCode=argomenti["-codeHighRes"];
    }
    if(argomenti["-help"]!=0){
        
        printHelp();
        return false;
    }
    return true;
    
}



void printHelp(){
    cout<<"-hzWrite: frequenza di aggiornamento simulata\n";
    cout<<"-fps: fps per i lettori\n";
    
    cout<<"-codeHighRes : numero di code in alta risoluzione (ne verranno create altrettante in bassa risoluzione)\n";
    cout<<"-threads: numero di thread lettori per ogni coda ad alta risoluzione, meta andranno nelle code ad alta risoluzione e meta in quelle lowRes\n";
    cout<<"-worktime: tempo di lavoro del thread in secondi\n";
    cout<<"-validity: millisecondi di validita di un elemento\n";
    cout<<"-cache : moltiplicatore per aumentare il numero di elementi che rimengono attivi\n";
    cout<<"-help : questo help\n";
    
    
    
}




int run(int argc, const char * argv[]){
    
    
    long hzGetData =2;
    long millisTimeCaching= 500;
    string dev="dev01";
    string aggregateDevice="dev01_agr";
    int numeroLettoriAlta=1;
    int numeroLettoriBassa=1;
    int lettoriAggregato=1;
    long subBufferValidityMillis=1000;
    long subBufferDiscretizationMillis=100;
    
    int aggragateBufferSize=10;
    long hzAggregateFetcher=2;
    
    
    std::stringstream path;
    
    
    
    
    path<<basePath;
    time_t inizio =time(0);
    path<<"_"<<inizio<<"/";
    basePath=path.str();
    mkdir(basePath.c_str(),0777);
    
    
    
    
    int fpsLettura=10;
    
    caching_system::DataFetcherInterface<Magnete>* fetcher=new SimpleDataFetcher();
    
    //caching_system::DataAggregationFilterInterface<Magnete, DataAggregate>* agr=new SimpleDataAggregator();
    
    
    caching_thread::ConcreteDeviceTracker<Magnete>* myTracker=new ConcreteDeviceTracker<Magnete>(fetcher,hzGetData,millisTimeCaching,dev);
    vector<SimpleDataReader*> lettori ;
    //vector<AggregateReader*> lettoriAggr ;
    
    
    
    
    
    boost::thread tracciatore(boost::ref(*myTracker));
    tracciatore.detach();
    
    for(int i=0;i<numeroLettoriAlta;i++){
        //IteratorReader<Magnete>* it, long fps,    std::string basePath,int id
        IteratorReader<Magnete>* lett=myTracker->openMaxResBuffer();
        SimpleDataReader* threadLettore=new SimpleDataReader(lett,fpsLettura,basePath,i,myTracker);
        lettori.push_back(threadLettore);
        
    }
    /*
    for(int i=numeroLettoriAlta;i<numeroLettoriAlta+numeroLettoriBassa;i++){
        //IteratorReader<Magnete>* it, long fps,    std::string basePath,int id
        //tienimeli per sei secondi e dammene uno al secondo
        long offsetmillis=0;
        /*if(i%2==0){
         offsetmillis=offsetmillis+i;
         }*/
/*        IteratorReader<Magnete>* lett=myTracker->openBuffer(subBufferValidityMillis+offsetmillis, subBufferDiscretizationMillis);
        SimpleDataReader* threadLettore=new SimpleDataReader(lett,fpsLettura,basePath,i,myTracker);
        lettori.push_back(threadLettore);
    }*/
    
    for(int i=0;i<lettori.size();i++){
        
        boost::thread lettore(boost::ref(* lettori.at(i)));
        lettore.detach();
        
        
        
    }
    
    
    /*caching_system::DataAggregationFilterInterface<Magnete, DataAggregate>* agregateFilter=new SimpleDataAggregator();
    
    IteratorReader<Magnete>* slaveReader =myTracker->openMaxResBuffer();
    caching_system::DataFetcherInterface<Magnete>* fetcherLocale=new LocalDataFetcher(slaveReader);
    
    caching_thread::AggregateDeviceTracker<Magnete,DataAggregate>* aggregateDeviceTracker=new AggregateDeviceTracker<Magnete,DataAggregate>(agregateFilter,fetcherLocale,hzAggregateFetcher,millisTimeCaching,aggregateDevice,aggragateBufferSize);
    
    boost::thread tracciatore2(boost::ref(*aggregateDeviceTracker));
    tracciatore2.detach();
    */
    
    
    //creo l'aggragto
 /*   for(int i=numeroLettoriAlta+numeroLettoriBassa;i<numeroLettoriAlta+numeroLettoriBassa+lettoriAggregato;i++){
        //IteratorReader<Magnete>* it, long fps,    std::string basePath,int id
        //tienimeli per sei secondi e dammene uno al secondo
        long offsetmillis=0;
        /*if(i%2==0){
         offsetmillis=offsetmillis+i;
         }*/
 /*       IteratorReader<DataAggregate>* lett=aggregateDeviceTracker->openBuffer(subBufferValidityMillis+offsetmillis, subBufferDiscretizationMillis);
        AggregateReader* threadLettore=new AggregateReader(lett,fpsLettura,basePath,i,aggregateDeviceTracker);
        lettoriAggr.push_back(threadLettore);
    }
    for(int i=0;i<lettoriAggr.size();i++){
        
        boost::thread lettore(boost::ref(* lettoriAggr.at(i)));
        lettore.detach();
        
        
        
    }*/
    
    
    
    boost::posix_time::seconds tempoWork(tempoDiLavoro);
    boost::this_thread::sleep(tempoWork);
    
    /*
    for(int i=0;i<lettori.size();i++){
        lettori.at(i)->interrupt();
    }
    for(int i=0;i<lettoriAggr.size();i++){
        lettoriAggr.at(i)->interrupt();
    }
    */
    boost::posix_time::seconds tempoWork2(5);
    boost::this_thread::sleep(tempoWork2);
    
    if(myTracker->tryInterruptTracking()){
        std::cout<<"chiuso con successo\n";
    }else{
        
        std::cout<<"qualcosa e' andato storto";
    }
    
   /* if(aggregateDeviceTracker->tryInterruptTracking()){
        std::cout<<"chiuso con successo\n";
    }else{
        
        std::cout<<"qualcosa e' andato storto";
    }*/
    std::cout<<"finito\n";
    
}


