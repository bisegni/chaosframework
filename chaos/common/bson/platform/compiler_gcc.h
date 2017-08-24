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

/**
 * Compiler-specific implementations for gcc.
 *
 * Refer to mongo/platform/compiler.h for usage documentation.
 */

#pragma once

#define BSON_MONGO_COMPILER_NORETURN __attribute__((__noreturn__))

#define BSON_MONGO_COMPILER_VARIABLE_UNUSED __attribute__((__unused__))

#define BSON_MONGO_COMPILER_ALIGN_TYPE(ALIGNMENT) __attribute__(( __aligned__(ALIGNMENT) ))

#define BSON_MONGO_COMPILER_ALIGN_VARIABLE(ALIGNMENT) __attribute__(( __aligned__(ALIGNMENT) ))

// NOTE(schwerin): These visibility and calling-convention macro definitions assume we're not using
// GCC/CLANG to target native Windows. If/when we decide to do such targeting, we'll need to change
// compiler flags on Windows to make sure we use an appropriate calling convention, and configure
// BSON_MONGO_COMPILER_API_EXPORT, BSON_MONGO_COMPILER_API_IMPORT and BSON_MONGO_COMPILER_API_CALLING_CONVENTION
// correctly.  I believe "correctly" is the following:
//
// #ifdef _WIN32
// #define BSON_MONGO_COMIPLER_API_EXPORT __attribute__(( __dllexport__ ))
// #define BSON_MONGO_COMPILER_API_IMPORT __attribute__(( __dllimport__ ))
// #ifdef _M_IX86
// #define BSON_MONGO_COMPILER_API_CALLING_CONVENTION __attribute__((__cdecl__))
// #else
// #define BSON_MONGO_COMPILER_API_CALLING_CONVENTION
// #endif
// #else ... fall through to the definitions below.

#define BSON_MONGO_COMPILER_API_EXPORT __attribute__(( __visibility__("default") ))
#define BSON_MONGO_COMPILER_API_IMPORT
#define BSON_MONGO_COMPILER_API_CALLING_CONVENTION
