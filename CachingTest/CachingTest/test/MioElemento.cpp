//
//  MioElemento.cpp
//  CachingSystem
//
//  Created by Flaminio Antonucci on 13/09/12.
//
//

#include "CachingTest/test/MioElemento.h"
Magnete::Magnete(double top,double down,double left,double right,long timestamp/*,long milliseconds_validity*/){
    
    this->top=top;
    this->down=down;
    this->left=left;
    this->right=right;
    this->timestamp=timestamp;
    // this->timeout=timestamp+milliseconds_validity;
    
}

double* Magnete::getTop(){
    return &top;
}
double* Magnete::getDown(){
    return &down;
}

double* Magnete::getLeft(){
    return &left;
}
double* Magnete::getRight(){
    return &right;
}

