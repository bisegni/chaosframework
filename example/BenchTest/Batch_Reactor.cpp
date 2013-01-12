/*
 
 il tempo di ciclo della control unit deve essere pari a 10ms (uguale a quello del controllo)
 
 dopo aver inizializzato il Batch_Reactor con le condizioni iniziali le chimate delle funzioni nel loop avvengono con il seguente ordine:
 
 
 compute_output;
 update_output;
 set_controllo;
 compute_state;
 
 
 */


#include <math.h>
#include "Batch_Reactor.h"


void Batch_Reactor::compute_state() {
	
	x[0] =  1.0142*x[0]-0.0018*x[1]+0.0651*x[2]-0.0546*x[3]+0.0000*u[0]-0.0010*u[1];
	x[1] = -0.0057*x[0]+0.9582*x[1]-0.0001*x[2]+0.0067*x[3]+0.0556*u[0]+0.0000*u[1];
	x[2] =  0.0103*x[0]+0.0417*x[1]+0.9363*x[2]+0.0563*x[3]+0.0125*u[0]-0.0304*u[1];
	x[3] =  0.0004*x[0]+0.0417*x[1]+0.0129*x[2]+0.9797*x[3]+0.0125*u[0]-0.0002*u[1];
}


void Batch_Reactor::compute_output() {
	y[0] = x[0]+x[2]-x[3];
	y[1] = x[1];
}


void Batch_Reactor::update_output() {
    
    double e[Q];
	
	e[0] = fabs(y_trasmessa[0] - y[0]);
	e[1] = fabs(y_trasmessa[1] - y[1]);
	
	if (e[0]>=e[1]) {//trasmetti uscita1 al MDS;
        y_trasmessa[0] = y[0];
    } else {
        //trasmetti uscita2 al MDS;
        y_trasmessa[1] = y[1];
    }
}


