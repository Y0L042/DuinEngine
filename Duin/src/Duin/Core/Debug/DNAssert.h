#pragma once

#include <doctest.h>
#include <functional>
#include <spdlog/fmt/fmt.h>

namespace duin
{
void InitAsserts();
void SetAssertContextCallback(std::function<std::string()> callback);
void OnCrash(const char *message);

/**
 * @brief Single sink for failed DN asserts: logs (with script callstack) but
 * never fails a doctest test. @param msg may be nullptr. Out-of-line in
 * Duin.lib so the call is identical in every TU (no ODR leak).
 */
void ReportAssert(const char *expr, const char *msg, const char *file, int line);

/**
 * @brief Format the assert message (fmt syntax) then forward to ReportAssert.
 * Header-inline formatting is fine — only the sink must be single-symbol.
 */
template <typename... Args>
inline void ReportAssertFmt(
    const char *expr, const char *file, int line, fmt::format_string<Args...> fmtStr, Args &&...args)
{
    ReportAssert(expr, fmt::format(fmtStr, std::forward<Args>(args)...).c_str(), file, line);
}

struct AssertContextScope
{
    explicit AssertContextScope(std::function<std::string()> cb)
    {
        SetAssertContextCallback(std::move(cb));
    }
    ~AssertContextScope()
    {
        SetAssertContextCallback(nullptr);
    }
    AssertContextScope(const AssertContextScope &) = delete;
    AssertContextScope &operator=(const AssertContextScope &) = delete;
};
} // namespace duin

// DN asserts route through duin::ReportAssert (defined once in Duin.lib) so the
// macro expands identically in every TU. The old CHECK/WARN expansion baked the
// fail-vs-log choice into header-inline code, which leaked via ODR from Duin.lib
// and failed unrelated DuinTests cases. ReportAssert logs but never fails a test
// (use doctest CHECK/REQUIRE for real expectations).
// DN_ALLOW_ASSERT_FAILS is now a no-op: log-don't-fail is the default.

// DN asserts behave like standard assert: active in debug builds, compiled out
// entirely in non-debug (release) builds. Use them for invariants/bugs only;
// for runtime failures that can legitimately happen, return std::expected.
// msg is an fmt-style format string; trailing args fill its {} placeholders:
//   DN_CORE_ASSERT(p != nullptr, "ptr {} was null at index {}", name, i);
#ifdef DN_DEBUG

#define DN_CORE_ASSERT(condition, ...)                                                                                 \
    ((condition) ? (void)0 : ::duin::ReportAssertFmt(#condition, __FILE__, __LINE__, __VA_ARGS__))
#define DN_CORE_ASSERT_NOMSG(condition)                                                                                \
    ((condition) ? (void)0 : ::duin::ReportAssert(#condition, nullptr, __FILE__, __LINE__))
#define DN_ASSERT(condition, ...) DN_CORE_ASSERT(condition, __VA_ARGS__)
#define DN_ASSERT_NOMSG(condition) DN_CORE_ASSERT_NOMSG(condition)

#else

#define DN_CORE_ASSERT(condition, ...) ((void)0)
#define DN_CORE_ASSERT_NOMSG(condition) ((void)0)
#define DN_ASSERT(condition, ...) ((void)0)
#define DN_ASSERT_NOMSG(condition) ((void)0)

#endif
