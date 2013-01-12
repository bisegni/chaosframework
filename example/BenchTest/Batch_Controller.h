/*
 
 Il valore del tempo di ciclo del loop lo devi impostare pari a 10 ms (uguale a quello della control unit)
 
 sequanza chiamata funzioni durante il loop:
 
 read();
 compute_controllo();
 
 */

#include "config.h"

class Batch_Controller {
	
	
	double e[Q]; //variabile ausiliaria per il calcolo del valore del controllo;
	double rif[Q]; //variabile per impostare un riferimento da far inseguire all'impianto;
	
	double e_precedente[Q]; // variabile ausiliaria;
	double u_precedente[Q]; // vairabile ausiliaria;
	
public:
    double u[P]; //variabile contenente il valore del controllo
	double y[Q]; //variabile contenente il valore dell'uscita misurata dell'impianto

	Batch_Controller();
	~Batch_Controller();
	
	void read();
	void compute_controllo();
	
	
};


