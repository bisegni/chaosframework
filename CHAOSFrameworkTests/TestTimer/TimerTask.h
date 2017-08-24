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

#ifndef __CHAOSFrameworkTests_A04DD2A1_4803_4A5E_A2B8_FC0D1BCD92D1_TimerTask_h
#define __CHAOSFrameworkTests_A04DD2A1_4803_4A5E_A2B8_FC0D1BCD92D1_TimerTask_h

#include <chaos/common/async_central/async_central.h>

class TimerTask:
public chaos::common::async_central::TimerHandler {
    int counter;
protected:
    void timeout();
public:
    TimerTask();
};

#endif /* __CHAOSFrameworkTests_A04DD2A1_4803_4A5E_A2B8_FC0D1BCD92D1_TimerTask_h */
