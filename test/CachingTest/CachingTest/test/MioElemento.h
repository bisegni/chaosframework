//
//  MioElemento.h
//  CachingSystem
//
//  Created by Flaminio Antonucci on 13/09/12.
//
//

#ifndef __CachingSystem__MioElemento__
#define __CachingSystem__MioElemento__

#include <iostream>


class Magnete{

    
public:
    double top;
    double down;
    double left;
    double right;
        // long timeout;
    long timestamp;
    
    Magnete(double top,double down,double left,double right,long timestamp);
    
    Magnete(){}
    double* getTop();
    double* getLeft();
    double* getRight();
    double* getDown();
    
    int     getValidity();
    friend void operator<<(std::ostream& out,const Magnete& mag){
        
        
        out<<"top: "<<mag.top<<"; ";
        out<<"down: "<<mag.down<<"; ";
        out<<"left: "<<mag.left<<"; ";
        out<<"right: "<<mag.right<<"; " ;
        out<<"timestamp: "<<(mag.timestamp)<<"; ";
        
        
        }
        
        // implementazione interfaccia
        long getTimestamp(){
            return timestamp;
        }

        
        };
        
        
#endif /* defined(__CachingSystem__MioElemento__) */
