/*
 * Copyright 2012, 2017 INFN
 *
 * Licensed under the EUPL, Version 1.2 or – as soon they
 * will be approved by the European Commission - subsequent
 * versions of the EUPL (the "Licence");
 * You may not use this work except in compliance with the
 * Licence.
 * You may obtain a copy of the Licence at:
 *
 * https://joinup.ec.europa.eu/software/page/eupl
 *
 * Unless required by applicable law or agreed to in
 * writing, software distributed under the Licence is
 * distributed on an "AS IS" basis,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied.
 * See the Licence for the specific language governing
 * permissions and limitations under the Licence.
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


