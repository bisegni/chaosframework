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
 * Include "mongo/platform/compiler.h" to get compiler-targeted macro definitions and utilities.
 *
 * The following macros are provided in all compiler environments:
 *
 *
 * BSON_MONGO_COMPILER_NORETURN
 *
 *   Instructs the compiler that the decorated function will not return through the normal return
 *   path.
 *
 *   Correct: BSON_MONGO_COMPILER_NORETURN void myAbortFunction();
 *
 *
 * BSON_MONGO_COMPILER_VARIABLE_UNUSED
 *
 *   Instructs the compiler not to warn if it detects no use of the decorated variable.
 *   Typically only useful for variables that are always declared but only used in
 *   conditionally-compiled code.
 *
 *   Correct: BSON_MONGO_COMPILER_VARIABLE_UNUSED int ignored;
 *
 *
 * BSON_MONGO_COMPILER_ALIGN_TYPE(ALIGNMENT)
 *
 *   Instructs the compiler to use the given minimum alignment for the decorated type.
 *
 *   Alignments should probably always be powers of two.  Also, note that most allocators will not
 *   be able to guarantee better than 16- or 32-byte alignment.
 *
 *   Correct:
 *     class BSON_MONGO_COMPILER_ALIGN_TYPE(16) MyClass {...};
 *
 *   Incorrect:
 *     BSON_MONGO_COMPILER_ALIGN_TYPE(16) class MyClass {...};
 *     class MyClass{...} BSON_MONGO_COMPILER_ALIGN_TYPE(16);
 *
 *
 * BSON_MONGO_COMPILER_ALIGN_VARIABLE(ALIGNMENT)
 *
 *   Instructs the compiler to use the given minimum alignment for the decorated variable.
 *
 *   Note that most allocators will not allow heap allocated alignments that are better than 16- or
 *   32-byte aligned.  Stack allocators may only guarantee up to the natural word length worth of
 *   alignment.
 *
 *   Correct:
 *     class MyClass {
 *         BSON_MONGO_COMPILER_ALIGN_VARIABLE(8) char a;
 *     };
 *
 *     BSON_MONGO_COMPILER_ALIGN_VARIABLE(8) class MyClass {...} singletonInstance;
 *
 *   Incorrect:
 *     int BSON_MONGO_COMPILER_ALIGN_VARIABLE(16) a, b;
 *
 *
 * BSON_MONGO_COMPILER_API_EXPORT
 *
 *   Instructs the compiler to label the given type, variable or function as part of the
 *   exported interface of the library object under construction.
 *
 *   Correct:
 *       BSON_MONGO_COMPILER_API_EXPORT int globalSwitch;
 *       class BSON_MONGO_COMPILER_API_EXPORT ExportedType { ... };
 *       BSON_MONGO_COMPILER_API_EXPORT SomeType exportedFunction(...);
 *
 *   NOTE: Rather than using this macro directly, one typically declares another macro named for the
 *   library, which is conditionally defined to either BSON_MONGO_COMIPLER_API_EXPORT or
 *   BSON_MONGO_COMPILER_API_IMPORT based on whether the compiler is currently building the library or
 *   building an object that depends on the library, respectively.  For example, BSON_MONGO_CLIENT_API
 *   might be defined to BSON_MONGO_COMPILER_API_EXPORT when building the MongoDB shared library, and to
 *   BSON_MONGO_COMPILER_API_IMPORT when building an application that links against the shared library.
 *
 *
 * BSON_MONGO_COMPILER_API_IMPORT
 *
 *   Instructs the compiler to label the given type, variable or function as imported
 *   from another library, and not part of the library object under construction.
 *
 *   Same correct/incorrect usage as for BSON_MONGO_COMPILER_API_EXPORT.
 *
 *
 * BSON_MONGO_COMPILER_API_CALLING_CONVENTION
 *
 *    Explicitly decorates a function declaration the api calling convention used for
 *    shared libraries.
 *
 *    Same correct/incorrect usage as for BSON_MONGO_COMPILER_API_EXPORT.
 */

#if defined(_MSC_VER)
#include <chaos/common/bson/platform/compiler_msvc.h>
#elif defined(__GNUC__)
#include <chaos/common/bson/platform/compiler_gcc.h>
#else
#error "Unsupported compiler family"
#endif
