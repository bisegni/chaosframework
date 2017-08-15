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


#include <math.h>
#include "Batch_Reactor.h"

Batch_Reactor::Batch_Reactor() {
    reset();
}


// FUNZIONE MODIFICATA!!!

void Batch_Reactor::compute_state() {
	
	double x1[N];
	
	noise.getNoise(d);
	for(int i=0;i<N;i++)
        x1[i] = A[i*N]*x[0]+A[i*N+1]*x[1]+A[i*N+2]*x[2]+A[i*N+3]*x[3]+B[i*P]*u[0]+B[i*P+1]*u[1]+B[i*P+2]*d[0]+B[i*P+3]*d[1];
    
    for(int i=0;i<N;i++)
     x[i]=x1[i];
	
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
    x[0] = 0.5;x[1] = 1;x[2] = 2;x[0] = 0.2;
    //memset(x, 0, N * sizeof(double));
    memset(y, 0, Q * sizeof(double));
    memset(u, 0, P * sizeof(double));
    memset(y_trasmessa, 0, Q * sizeof(double));
}
