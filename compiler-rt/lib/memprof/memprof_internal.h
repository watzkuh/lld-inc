//===-- memprof_internal.h -------------------------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file is a part of MemProfiler, a memory profiler.
//
// MemProf-private header which defines various general utilities.
//===----------------------------------------------------------------------===//
#ifndef MEMPROF_INTERNAL_H
#define MEMPROF_INTERNAL_H

#include "memprof_flags.h"
#include "memprof_interface_internal.h"
#include "sanitizer_common/sanitizer_common.h"
#include "sanitizer_common/sanitizer_internal_defs.h"
#include "sanitizer_common/sanitizer_libc.h"
#include "sanitizer_common/sanitizer_stacktrace.h"

#if __has_feature(address_sanitizer) || defined(__SANITIZE_ADDRESS__)
#error "The MemProfiler run-time should not be instrumented by MemProfiler"
#endif

// Build-time configuration options.

// If set, memprof will intercept C++ exception api call(s).
#ifndef MEMPROF_HAS_EXCEPTIONS
#define MEMPROF_HAS_EXCEPTIONS 1
#endif

#ifndef MEMPROF_DYNAMIC
#ifdef PIC
#define MEMPROF_DYNAMIC 1
#else
#define MEMPROF_DYNAMIC 0
#endif
#endif

// All internal functions in memprof reside inside the __memprof namespace
// to avoid namespace collisions with the user programs.
// Separate namespace also makes it simpler to distinguish the memprof
// run-time functions from the instrumented user code in a profile.
namespace __memprof {

class MemprofThread;
using __sanitizer::StackTrace;

void MemprofInitFromRtl();

// memprof_rtl.cpp
void PrintAddressSpaceLayout();

// memprof_shadow_setup.cpp
void InitializeShadowMemory();

// memprof_malloc_linux.cpp
void ReplaceSystemMalloc();

// memprof_linux.cpp
uptr FindDynamicShadowStart();
void *MemprofDoesNotSupportStaticLinkage();

// memprof_thread.cpp
MemprofThread *CreateMainThread();

void ReadContextStack(void *context, uptr *stack, uptr *ssize);

// Wrapper for TLS/TSD.
void TSDInit(void (*destructor)(void *tsd));
void *TSDGet();
void TSDSet(void *tsd);
void PlatformTSDDtor(void *tsd);

void *MemprofDlSymNext(const char *sym);

// Add convenient macro for interface functions that may be represented as
// weak hooks.
#define MEMPROF_MALLOC_HOOK(ptr, size)                                         \
  do {                                                                         \
    if (&__sanitizer_malloc_hook)                                              \
      __sanitizer_malloc_hook(ptr, size);                                      \
    RunMallocHooks(ptr, size);                                                 \
  } while (false)
#define MEMPROF_FREE_HOOK(ptr)                                                 \
  do {                                                                         \
    if (&__sanitizer_free_hook)                                                \
      __sanitizer_free_hook(ptr);                                              \
    RunFreeHooks(ptr);                                                         \
  } while (false)

extern int memprof_inited;
extern int memprof_timestamp_inited;
extern int memprof_init_done;
// Used to avoid infinite recursion in __memprof_init().
extern bool memprof_init_is_running;
extern void (*death_callback)(void);
extern long memprof_init_timestamp_s;

} // namespace __memprof

#endif // MEMPROF_INTERNAL_H
