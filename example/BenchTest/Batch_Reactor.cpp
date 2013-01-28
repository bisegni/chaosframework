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

Batch_Reactor::Batch_Reactor() {
    reset();
}


// FUNZIONE MODIFICATA!!!

void Batch_Reactor::compute_state() {
	noise.getNoise(d);
	for(int i=0;i<N;i++)
        x[i] = A[i*N]*x[0]+A[i*N+1]*x[1]+A[i*N+2]*x[2]+A[i*N+3]*x[3]+B[i*P]*u[0]+B[i*P+1]*u[1]+B[i*P+2]*d;
    
}


// FUNZIONE MODIFICATA!!!

void Batch_Reactor::compute_output() {
	for(int i=0;i<Q;i++)
        y[i] = C[i*N]*x[0]+C[i*N+1]*x[1]+C[i*N+2]*x[2]+C[i*N+3]*x[3];
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

void Batch_Reactor::reset() {
    memset(x, 0, N * sizeof(double));
    memset(y, 0, Q * sizeof(double));
    memset(u, 0, P * sizeof(double));
    memset(y_trasmessa, 0, Q * sizeof(double));
}