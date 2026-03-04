#pragma once

#include <doctest.h>

namespace duin
{
void InitAsserts();
} // namespace duin

// DN_CORE_ASSERT / DN_ASSERT: always active.
// Inside a doctest test case  -> CHECK (test failure) or WARN (logged only).
// Outside a test case (runtime) -> calls the assert handler set by InitAsserts().
//
// Define DN_ALLOW_ASSERT_FAILS as a project-wide build define (e.g. in DuinTests)
// to downgrade all DN asserts to WARN level — failures are logged but do NOT
// count as test failures. Use this for test projects where assert firing is an
// expected and intentional behaviour being verified.

#ifndef DN_ALLOW_ASSERT_FAILS

#define DN_CORE_ASSERT(condition, msg)  CHECK_MESSAGE(condition, msg)
#define DN_CORE_ASSERT_NOMSG(condition) CHECK(condition)
#define DN_ASSERT(condition, msg)       CHECK_MESSAGE(condition, msg)
#define DN_ASSERT_NOMSG(condition)      CHECK(condition)

#else

#define DN_CORE_ASSERT(condition, msg)  WARN_MESSAGE(condition, msg)
#define DN_CORE_ASSERT_NOMSG(condition) WARN(condition)
#define DN_ASSERT(condition, msg)       WARN_MESSAGE(condition, msg)
#define DN_ASSERT_NOMSG(condition)      WARN(condition)

#endif

// Debug-only variants: compiled out in non-debug builds.
#ifdef DN_DEBUG

#define DN_CORE_DEBUG_ASSERT(condition, msg) CHECK_MESSAGE(condition, msg)
#define DN_CORE_DEBUG_ASSERT_NOMSG(condition) CHECK(condition)
#define DN_DEBUG_ASSERT(condition, msg) CHECK_MESSAGE(condition, msg)
#define DN_DEBUG_ASSERT_NOMSG(condition) CHECK(condition)

#else

#define DN_CORE_DEBUG_ASSERT(condition, msg) ((void)0)
#define DN_CORE_DEBUG_ASSERT_NOMSG(condition) ((void)0)
#define DN_DEBUG_ASSERT(condition, msg) ((void)0)
#define DN_DEBUG_ASSERT_NOMSG(condition) ((void)0)

#endif

// Verify variants: condition always evaluated; only the failure action is debug-gated.
#ifdef DN_DEBUG

#define DN_CORE_VERIFY(condition, msg) CHECK_MESSAGE(condition, msg)
#define DN_VERIFY(condition, msg) CHECK_MESSAGE(condition, msg)

#else

#define DN_CORE_VERIFY(condition, msg) ((void)(condition))
#define DN_VERIFY(condition, msg) ((void)(condition))

#endif
