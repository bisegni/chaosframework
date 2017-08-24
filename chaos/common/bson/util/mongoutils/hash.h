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

/*    Copyright 2009 10gen Inc.
 *
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */

#pragma once

namespace bsonutils {

    /** @return hash of a pointer to an unsigned. so you get a 32 bit hash out, regardless of whether
                pointers are 32 or 64 bit on the particular platform.

        is there a faster way to impl this that hashes just as well?
    */
    inline unsigned hashPointer(void *v) {
        unsigned x = 0;
        unsigned char *p = (unsigned char *) &v;
        for( unsigned i = 0; i < sizeof(void*); i++ ) {
            x = x * 131 + p[i];
        }
        return x;
    }

    inline unsigned hash(unsigned u) {
        unsigned char *p = (unsigned char *) &u;
        return (((((p[3] * 131) + p[2]) * 131) + p[1]) * 131) + p[0];
    }

}
