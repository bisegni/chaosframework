#ifndef ChaosFramework_noise
#define ChaosFramework_noise
#include <math.h>
#include "config.h"

#define SWITCH_TIME         15000000 //15sec



class Disturbo {
    double xd[2];
	double d[2];
	double omega;
	int n;
    
    public:
    
    Disturbo();
	void getNoise(double []);
};
#endif

