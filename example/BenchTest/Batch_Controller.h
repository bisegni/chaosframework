/*
 
 Il valore del tempo di ciclo del loop lo devi impostare pari a 10 ms (uguale a quello della control unit)
 
 sequanza chiamata funzioni durante il loop:
 
 read();
 compute_controllo();
 
 */

#include "config.h"
#include <math.h>
#include <boost/random.hpp>

class Batch_Controller {
	
	
	double w[Pc]; //variabile ausiliaria per il calcolo del valore del controllo;
	
	
protected:
    double rif[Q]; //variabile per impostare un riferimento da far inseguire all'impianto;
    
public:
    
	double xc[Nc]; //vettore di stato del controllo;
    double u[Qc]; //variabile contenente il valore del controllo;
	double y[Q]; //variabile contenente il valore dell'uscita misurata dell'impianto;
    
	Batch_Controller();
	virtual ~Batch_Controller();
	
	void read();
	void compute_state();
	void compute_controllo();
	
	
};


