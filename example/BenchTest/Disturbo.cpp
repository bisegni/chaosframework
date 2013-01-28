#include <math.h>
#include <stdlib.h>
#include <cstring>

#include "Disturbo.h"

Disturbo::Disturbo() {
    memset(xd, 0, 2 * sizeof(double));
    memset(d, 0, 2 * sizeof(double));
    n = 0;
    
}


void Disturbo::getNoise(double& app) {
	
	if(n%(SWITCH_TIME/CU_DELAY_FROM_TASKS)== 0)
	 {   
      xd[0] = rand()%10+1;
	 };
	
	d[0] = 0.00497512437810945*xd[0]+0.00497512437810945*xd[1]+0.990049751243781*d[1];
	
	
	d[1]=d[0];
	xd[1]=xd[0];	

    n = n+1;

	app = d[0];

	
}




