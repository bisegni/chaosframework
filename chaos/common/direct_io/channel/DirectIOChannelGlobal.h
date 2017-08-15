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
#ifndef CHAOSFramework_ChannelGlobal_h
#define CHAOSFramework_ChannelGlobal_h
namespace chaos {
	namespace common {
		namespace direct_io {
			namespace channel {
				//index of all channel
				#define DIOCDC_Channel_Index			1
				#define DIODataset_Channel_Index		2
				#define DIOPerformance_Channel_Index	3
				#define DIOSystemAPI_Channel_Index		4
				
				
#define DELETE_HEADER_DATA(h,d)\
if(h) free(h);\
if(d) free(d);
				
#define DELETE_HEADER(h)\
if(h) free(h);
            }
        }
    }
}
#endif
