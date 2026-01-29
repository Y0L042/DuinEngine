/**
 * @file DNAssert.h
 * @brief Assertion macros for debugging and error checking.
 * @ingroup Core_Debug
 *
 * Provides assertion macros that log detailed error messages before failing.
 * These macros can be used in expressions, just like standard assert().
 *
 * Two variants are available:
 * - DN_ASSERT / DN_CORE_ASSERT: Always active, use for critical invariants
 * - DN_DEBUG_ASSERT / DN_CORE_DEBUG_ASSERT: Only active in debug builds
 *
 * @code
 * DN_ASSERT(ptr != nullptr, "Pointer must not be null");
 * DN_CORE_ASSERT(index < size, "Index out of bounds");
 *
 * // Can be used in expressions:
 * int value = (DN_ASSERT(x > 0, "x must be positive"), x);
 * @endcode
 */

#pragma once

#include "DNLog.h"

// Platform-specific debug break
#if defined(_MSC_VER)
#define DN_DEBUG_BREAK() __debugbreak()
#elif defined(__clang__) || defined(__GNUC__)
#define DN_DEBUG_BREAK() __builtin_trap()
#else
#include <csignal>
#define DN_DEBUG_BREAK() std::raise(SIGTRAP)
#endif

// Internal helper that logs and breaks, returns void
#define DN_ASSERT_FAIL_CORE_(condition, msg)                                                                           \
    (DN_CORE_FATAL("Assertion failed: {}", #condition),                                                                \
     DN_CORE_FATAL("  Location: {}:{} in {}", __FILE__, __LINE__, __func__), DN_CORE_FATAL("  Message: {}", msg),      \
     DN_DEBUG_BREAK())

#define DN_ASSERT_FAIL_(condition, msg)                                                                                \
    (DN_FATAL("Assertion failed: {}", #condition), DN_FATAL("  Location: {}:{} in {}", __FILE__, __LINE__, __func__),  \
     DN_FATAL("  Message: {}", msg), DN_DEBUG_BREAK())

/**
 * @name Core Assert Macros
 * For engine/core code assertions. Always active.
 * @{
 */

#define DN_CORE_ASSERT(condition, msg) ((condition) ? (void)0 : DN_ASSERT_FAIL_CORE_(condition, msg))

#define DN_CORE_ASSERT_NOMSG(condition) ((condition) ? (void)0 : DN_ASSERT_FAIL_CORE_(condition, ""))

/** @} */

/**
 * @name Client Assert Macros
 * For application code assertions. Always active.
 * @{
 */

#define DN_ASSERT(condition, msg) ((condition) ? (void)0 : DN_ASSERT_FAIL_(condition, msg))

#define DN_ASSERT_NOMSG(condition) ((condition) ? (void)0 : DN_ASSERT_FAIL_(condition, ""))

/** @} */

/**
 * @name Debug-Only Assert Macros
 * Only active when DN_DEBUG is defined. Compile to nothing in release builds.
 * @{
 */

#ifdef DN_DEBUG

#define DN_CORE_DEBUG_ASSERT(condition, msg) DN_CORE_ASSERT(condition, msg)
#define DN_CORE_DEBUG_ASSERT_NOMSG(condition) DN_CORE_ASSERT_NOMSG(condition)
#define DN_DEBUG_ASSERT(condition, msg) DN_ASSERT(condition, msg)
#define DN_DEBUG_ASSERT_NOMSG(condition) DN_ASSERT_NOMSG(condition)

#else

#define DN_CORE_DEBUG_ASSERT(condition, msg) ((void)0)
#define DN_CORE_DEBUG_ASSERT_NOMSG(condition) ((void)0)
#define DN_DEBUG_ASSERT(condition, msg) ((void)0)
#define DN_DEBUG_ASSERT_NOMSG(condition) ((void)0)

#endif

/** @} */

/**
 * @name Verify Macros
 * Like asserts, but always evaluate the condition (even in release).
 * Use when the condition has side effects that must always execute.
 * @{
 */

#ifdef DN_DEBUG

#define DN_VERIFY(condition, msg) ((condition) ? (void)0 : DN_ASSERT_FAIL_(condition, msg))

#define DN_CORE_VERIFY(condition, msg) ((condition) ? (void)0 : DN_ASSERT_FAIL_CORE_(condition, msg))

#else

#define DN_VERIFY(condition, msg) ((void)(condition))
#define DN_CORE_VERIFY(condition, msg) ((void)(condition))

#endif

/** @} */
