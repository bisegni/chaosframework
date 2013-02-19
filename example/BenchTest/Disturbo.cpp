#include <math.h>
#include <stdlib.h>
#include <cstring>

#include "Disturbo.h"

Disturbo::Disturbo() {
    memset(xd, 0, 2 * sizeof(double));
    memset(d, 0, 2 * sizeof(double));
    n = 0;
    omega = 62.83185; //10Hz
    
}


void Disturbo::getNoise(double app[]) {
	
	if(n%(SWITCH_TIME/CU_DELAY_FROM_TASKS)== 0)
	 {   
      xd[0] = rand()%10+1;
	 };
	
	d[0] = 1;//0.00497512437810945*xd[0]+0.00497512437810945*xd[1]+0.990049751243781*d[1];
	
	
	d[1]=d[0];
	xd[1]=xd[0];
	
	app[0] = d[0];	
	app[1] = sin(omega*n*CU_DELAY_FROM_TASKS);

    n = n+1;

		
}




