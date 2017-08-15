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
#ifndef ChaosFramework_reactor
#define ChaosFramework_reactor
#include <boost/random.hpp>
#include <math.h>
#include "config.h"
#include "Disturbo.h"

using namespace boost;

class Batch_Reactor {
    double error;
    Disturbo noise;
public:
    double x[N]; // initial condition
	double y[Q]; // reactor output
	double u[P]; // control value
    double y_trasmessa[Q];
	double d[2];
	
    Batch_Reactor();
	void compute_output();
	void compute_state(); //non modificare il tempo di campionamente DeltaT!!!
	void update_output();
    void reset();
};
#endif

