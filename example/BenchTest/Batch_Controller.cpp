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

#include <string>
#include "Batch_Controller.h"


// FUNZIONE MODIFICATA!!!

Batch_Controller::Batch_Controller()
{
    
    memset(rif, 0, Q * sizeof(double));
    memset(xc, 0, Nc * sizeof(double));
    memset(y, 0, Q * sizeof(double));
    memset(u, 0, Qc * sizeof(double));
    memset(w, 0, Pc * sizeof(double));
}

Batch_Controller::~Batch_Controller() {
    
}

// FUNZIONE AGGIUNTA!!!

void Batch_Controller::compute_state() {
	
	double xc1[Nc];
	
	for(int i=0;i<Nc;i++)
        xc1[i] = Ac[i*Nc]*xc[0]+Ac[i*Nc+1]*xc[1]+Ac[i*Nc+2]*xc[2]+Ac[i*Nc+3]*xc[3]+Ac[i*Nc+4]*xc[4]+Ac[i*Nc+5]*xc[5]+Ac[i*Nc+6]*xc[6]
        +Ac[i*Nc+7]*xc[7]+Ac[i*Nc+8]*xc[8]+Ac[i*Nc+9]*xc[9]+Ac[i*Nc+10]*xc[10]+Ac[i*Nc+11]*xc[11]
        +Bc[i*Pc]*w[0]+Bc[i*Pc+1]*w[1]+Bc[i*Pc+2]*w[2]+Bc[i*Pc+3]*w[3];
    
    for(int i=0;i<Nc;i++)
     xc[i]=xc1[i];
     
}


// FUNZIONE MODIFICATA!!!

void Batch_Controller::compute_controllo() {
    
	w[0] = rif[0]-y[0];
	w[1] = rif[1]-y[1];
	w[2] = y[0];
	w[3] = y[1];
	
	
	for(int i=0;i<Qc;i++)
	{
        u[i] = Cc[i*Nc]*xc[0]+Cc[i*Nc+1]*xc[1]+Cc[i*Nc+2]*xc[2]+Cc[i*Nc+3]*xc[3]+Cc[i*Nc+4]*xc[4]+Cc[i*Nc+5]*xc[5]+Cc[i*Nc+6]*xc[6]
        +Cc[i*Nc+7]*xc[7]+Cc[i*Nc+8]*xc[8]+Cc[i*Nc+9]*xc[9]+Cc[i*Nc+10]*xc[10]+Cc[i*Nc+11]*xc[11]+
        Dc[i*Pc]*w[0]+Dc[i*Pc+1]*w[1]+Dc[i*Pc+2]*w[2]+Dc[i*Pc+3]*w[3];
	}
}



