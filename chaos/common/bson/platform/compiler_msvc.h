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
 * Compiler-specific implementations for MSVC.
 *
 * Refer to mongo/platform/compiler.h for usage documentation.
 */

#pragma once

#define BSON_MONGO_COMPILER_NORETURN __declspec(noreturn)

#define BSON_MONGO_COMPILER_VARIABLE_UNUSED

#define BSON_MONGO_COMPILER_ALIGN_TYPE(ALIGNMENT) __declspec( align( ALIGNMENT ) )

#define BSON_MONGO_COMPILER_ALIGN_VARIABLE(ALIGNMENT) __declspec( align( ALIGNMENT ) )

#define BSON_MONGO_COMPILER_API_EXPORT __declspec(dllexport)
#define BSON_MONGO_COMPILER_API_IMPORT __declspec(dllimport)

#ifdef _M_IX86
// 32-bit x86 supports multiple of calling conventions.  We build supporting the cdecl convention
// (most common).  By labeling our exported and imported functions as such, we do a small favor to
// 32-bit Windows developers.
#define BSON_MONGO_COMPILER_API_CALLING_CONVENTION __cdecl
#else
#define BSON_MONGO_COMPILER_API_CALLING_CONVENTION
#endif
