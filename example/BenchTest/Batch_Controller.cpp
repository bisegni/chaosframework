/*
 
 Il valore del tempo di ciclo del loop lo devi impostare pari a 10 ms (uguale a quello della control unit)
 
 sequanza chiamata funzioni durante il loop:
 
 read();
 compute_controllo();
 
 */

#include <cstring>
#include "Batch_Controller.h"

using namespace std;

Batch_Controller::Batch_Controller()
{

    memset(rif, 0, Q * sizeof(double));
    memset(y, 0, Q * sizeof(double));
    memset(u, 0, P * sizeof(double));
    memset(e_precedente, 0, Q * sizeof(double));
    memset(u_precedente, 0, Q * sizeof(double));
    memset(e_precedente, 0, Q * sizeof(double));
}

Batch_Controller::~Batch_Controller() {
    
}


void Batch_Controller::compute_controllo() {
    
	e[0] = rif[0] - y[0];
	e[1] = rif[1] - y[1];
    
	u[0] = u_precedente[0] + 2.01*e[1] - 1.99*e_precedente[1];
	u[1] = u_precedente[1] -5.04*e[0] +  4.96*e_precedente[0];
    
	u_precedente[0] = u[0];
	u_precedente[1] = u[1];
    
	e_precedente[0] = e[0];
	e_precedente[1] = e[1];
}



