//
//  dataAggragate.h
//  CachingSystem
//
//  Created by Flaminio Antonucci on 15/11/12.
//
//

#ifndef CachingSystem_dataAggragate_h
#define CachingSystem_dataAggragate_h

class DataAggregate {
    
    double top,down,left,right;
    
    
public:
    
    DataAggregate(){}
    
    DataAggregate(double top,double down,double left,double right){
        this->top=top;
        this->down=down;
        this->left=left;
        this->right=right;
    
    
    }
    
    friend void operator<<(std::ostream& out,const DataAggregate& mag){
        
        
        out<<"canale1: "<<mag.top<<"; ";
        out<<"canale2: "<<mag.down<<"; ";
        out<<"canale3: "<<mag.left<<"; ";
        out<<"canale4: "<<mag.right<<"; " ;
       // out<<"timestamp: "<<(mag.timestamp)<<"; ";
        
        
        }
        

    
};

#endif
