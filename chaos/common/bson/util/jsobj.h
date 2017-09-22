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

/**
   BSONObj and its helpers

   "BSON" stands for "binary JSON" -- ie a binary way to represent objects that would be
   represented in JSON (plus a few extensions useful for databases & other languages).

   http://www.bsonspec.org/
*/

#pragma once

#include <chaos/common/bson/util/builder.h>
#include <chaos/common/bson/optime.h>
#include <chaos/common/bson/bsontypes.h>
#include <chaos/common/bson/oid.h>
#include <chaos/common/bson/bsonelement.h>
#include <chaos/common/bson/bsonobj.h>
#include <chaos/common/bson/bsonmisc.h>
#include <chaos/common/bson/bsonobjbuilder.h>
#include <chaos/common/bson/bsonobjiterator.h>
#include <chaos/common/bson/bson-inl.h>
#include <chaos/common/bson/ordering.h>
#include <chaos/common/bson/base/string_data.h>
#include <chaos/common/bson/bson_db.h>

