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

#pragma once

/**
 * Instruct the compiler not to create default copy constructor and assignment operator
 * for class "CLASS".  Must be the _first_ or _last_ line of the class declaration.  Prefer
 * to use it as the first line.
 *
 * Usage:
 *    class Foo {
 *        BSON_MONGO_DISALLOW_COPYING(Foo);
 *    public:
 *        ...
 *    };
 */
#define BSON_MONGO_DISALLOW_COPYING(CLASS) \
    private:                                    \
    CLASS(const CLASS&);                        \
    CLASS& operator=(const CLASS&)
