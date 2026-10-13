#include "SignalBenchmark.h"

#include <doctest.h>
#include <Duin/Core/Signals/Signal.h>

#include <algorithm>
#include <array>
#include <cmath>
#include <ctime>
#include <format>
#include <iostream>
#include <memory>
#include <numeric>
#include <string>
#include <vector>

namespace duin::bench
{

namespace
{

volatile std::int64_t g_sink = 0;

std::string MakeTimestamp(const char *dateTimeSep)
{
    auto now = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);
    std::tm *local = std::localtime(&t);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count() % 1000;
    return std::format(
        "{:04}-{:02}-{:02}{}{:02}-{:02}-{:02}-{:03}",
        1900 + local->tm_year,
        1 + local->tm_mon,
        local->tm_mday,
        dateTimeSep,
        local->tm_hour,
        local->tm_min,
        local->tm_sec,
        ms);
}

std::filesystem::path MakeUniqueLogPath(const std::filesystem::path &directory, const std::string &prefix)
{
    std::filesystem::create_directories(directory);
    std::string stamp = MakeTimestamp("_");
    std::filesystem::path path = directory / std::format("{}_{}.log", prefix, stamp);
    for (int i = 1; std::filesystem::exists(path); ++i)
    {
        path = directory / std::format("{}_{}_{}.log", prefix, stamp, i);
    }
    return path;
}

const char *BuildConfigName()
{
#if defined(DN_DEBUG)
    return "Debug";
#elif defined(DN_RELEASE)
    return "Release";
#elif defined(DN_DIST)
    return "Dist";
#elif defined(DN_ARCHIVE)
    return "Archive";
#else
    return "Unknown";
#endif
}

std::string CompilerName()
{
#if defined(__clang__)
    return std::format("Clang {}.{}.{}", __clang_major__, __clang_minor__, __clang_patchlevel__);
#elif defined(_MSC_VER)
    return std::format("MSVC {}", _MSC_FULL_VER);
#elif defined(__GNUC__)
    return std::format("GCC {}.{}.{}", __GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__);
#else
    return "Unknown";
#endif
}

BenchmarkStats ComputeStats(const std::string &name, const BenchmarkConfig &config, std::vector<double> nsPerOp)
{
    BenchmarkStats stats;
    stats.name = name;
    stats.samples = nsPerOp.size();
    stats.opsPerSample = config.opsPerSample;
    stats.itemsPerOp = config.itemsPerOp;
    if (nsPerOp.empty())
        return stats;

    std::sort(nsPerOp.begin(), nsPerOp.end());
    size_t n = nsPerOp.size();
    stats.minNs = nsPerOp.front();
    stats.maxNs = nsPerOp.back();
    stats.medianNs = (n % 2 == 1) ? nsPerOp[n / 2] : (nsPerOp[n / 2 - 1] + nsPerOp[n / 2]) * 0.5;
    stats.meanNs = std::accumulate(nsPerOp.begin(), nsPerOp.end(), 0.0) / static_cast<double>(n);

    double variance = 0.0;
    for (double v : nsPerOp)
        variance += (v - stats.meanNs) * (v - stats.meanNs);
    stats.stddevNs = std::sqrt(variance / static_cast<double>(n));
    return stats;
}

} // namespace

void Consume(std::int64_t value)
{
    g_sink = value;
}

BenchmarkLog::BenchmarkLog(const std::filesystem::path &directory, const std::string &prefix)
    : path_(MakeUniqueLogPath(directory, prefix)), file_(path_, std::ios::out | std::ios::trunc),
      start_(std::chrono::steady_clock::now())
{
    WriteHeader();
}

BenchmarkLog::~BenchmarkLog()
{
    auto elapsed = std::chrono::duration<double>(std::chrono::steady_clock::now() - start_).count();
    WriteLine("");
    WriteLine(std::format("Finished in {:.3f} s", elapsed));
}

void BenchmarkLog::Section(const std::string &title)
{
    WriteLine("");
    WriteLine(std::format("== {} ==", title));
    WriteTableHeader();
}

void BenchmarkLog::Note(const std::string &text)
{
    WriteLine(std::format("  # {}", text));
}

void BenchmarkLog::Record(const BenchmarkStats &stats)
{
    double nsPerItem = stats.medianNs / static_cast<double>(std::max<size_t>(stats.itemsPerOp, 1));
    double opsPerSec = stats.medianNs > 0.0 ? 1e9 / stats.medianNs : 0.0;
    WriteLine(
        std::format(
            "{:<58} {:>8} {:>8} {:>12.2f} {:>12.2f} {:>12.2f} {:>12.2f} {:>10.2f} {:>11.3f} {:>14.0f}",
            stats.name,
            stats.samples,
            stats.opsPerSample,
            stats.minNs,
            stats.medianNs,
            stats.meanNs,
            stats.maxNs,
            stats.stddevNs,
            nsPerItem,
            opsPerSec));
}

const std::filesystem::path &BenchmarkLog::GetPath() const
{
    return path_;
}

void BenchmarkLog::WriteHeader()
{
    WriteLine("Duin Signal Benchmark");
    WriteLine(std::format("Started:  {}", MakeTimestamp(" ")));
    WriteLine(std::format("Build:    {}", BuildConfigName()));
    WriteLine(std::format("Compiler: {}", CompilerName()));
    WriteLine(std::format("Log file: {}", std::filesystem::absolute(path_).string()));
    WriteLine("All timings are nanoseconds per op. Stats are taken across samples. ns/item = median / items per op.");
}

void BenchmarkLog::WriteTableHeader()
{
    WriteLine(
        std::format(
            "{:<58} {:>8} {:>8} {:>12} {:>12} {:>12} {:>12} {:>10} {:>11} {:>14}",
            "Benchmark",
            "samples",
            "ops",
            "min",
            "median",
            "mean",
            "max",
            "stddev",
            "ns/item",
            "ops/s(median)"));
    WriteLine(std::string(58 + 8 + 8 + 12 * 4 + 10 + 11 + 14 + 9, '-'));
}

void BenchmarkLog::WriteLine(const std::string &line)
{
    std::cout << line << '\n';
    if (file_.is_open())
    {
        file_ << line << '\n';
        file_.flush();
    }
}

BenchmarkLog &GetSignalBenchmarkLog()
{
    static BenchmarkLog log("../../../BenchmarkLogs", "signal_benchmark");
    return log;
}

BenchmarkStats RunBenchmark(
    const std::string &name, const BenchmarkConfig &config, const std::function<void()> &setup,
    const std::function<void()> &body, const std::function<void()> &teardown)
{
    using Clock = std::chrono::steady_clock;

    std::vector<double> nsPerOp;
    nsPerOp.reserve(config.samples);

    size_t total = config.warmupSamples + config.samples;
    for (size_t i = 0; i < total; ++i)
    {
        if (setup)
            setup();

        auto begin = Clock::now();
        body();
        auto end = Clock::now();

        if (teardown)
            teardown();

        if (i >= config.warmupSamples)
        {
            double ns = std::chrono::duration<double, std::nano>(end - begin).count();
            nsPerOp.push_back(ns / static_cast<double>(std::max<size_t>(config.opsPerSample, 1)));
        }
    }

    return ComputeStats(name, config, std::move(nsPerOp));
}

BenchmarkStats RunAndRecord(
    const std::string &name, const BenchmarkConfig &config, const std::function<void()> &setup,
    const std::function<void()> &body, const std::function<void()> &teardown)
{
    BenchmarkStats stats = RunBenchmark(name, config, setup, body, teardown);
    GetSignalBenchmarkLog().Record(stats);
    return stats;
}

} // namespace duin::bench

namespace
{

using namespace duin::bench;

constexpr std::array<size_t, 4> kListenerCounts = {1, 10, 100, 1000};
constexpr std::array<size_t, 5> kEmitListenerCounts = {0, 1, 10, 100, 1000};
constexpr size_t kTargetCallsPerSample = 200000;

size_t EmitsFor(size_t listeners)
{
    return std::max<size_t>(1000, kTargetCallsPerSample / std::max<size_t>(listeners, 1));
}

struct LargePayload
{
    std::array<std::int64_t, 16> values{};
};

} // namespace

TEST_SUITE("SignalBenchmarks" * doctest::skip())
{

    TEST_CASE("Signal benchmark: construction")
    {
        auto &log = GetSignalBenchmarkLog();
        log.Section("Construction / destruction");

        constexpr size_t count = 10000;
        RunAndRecord("Signal<int> construct+destroy", {.samples = 30, .opsPerSample = count}, nullptr, [&] {
            std::int64_t acc = 0;
            for (size_t i = 0; i < count; ++i)
            {
                duin::Signal<int> sig;
                acc += static_cast<std::int64_t>(sig.GetListenerCount());
            }
            Consume(acc);
        });

        RunAndRecord("Signal<int> move-construct", {.samples = 30, .opsPerSample = count}, nullptr, [&] {
            std::int64_t acc = 0;
            for (size_t i = 0; i < count; ++i)
            {
                duin::Signal<int> a;
                duin::Signal<int> b(std::move(a));
                acc += static_cast<std::int64_t>(b.GetListenerCount());
            }
            Consume(acc);
        });
    }

    TEST_CASE("Signal benchmark: connect")
    {
        auto &log = GetSignalBenchmarkLog();
        log.Section("Connect");

        for (size_t n : kListenerCounts)
        {
            std::unique_ptr<duin::Signal<int>> sig;
            std::int64_t counter = 0;
            RunAndRecord(
                std::format("Connect x{}", n),
                {.samples = 50, .opsPerSample = n},
                [&] { sig = std::make_unique<duin::Signal<int>>(); },
                [&] {
                    for (size_t i = 0; i < n; ++i)
                        sig->Connect([&counter](int v) { counter += v; });
                },
                [&] {
                    CHECK(sig->GetListenerCount() == n);
                    sig.reset();
                });
        }

        for (size_t n : kListenerCounts)
        {
            std::unique_ptr<duin::Signal<int>> sig;
            std::vector<std::shared_ptr<duin::ScopedConnection>> conns;
            std::int64_t counter = 0;
            RunAndRecord(
                std::format("ConnectScoped x{}", n),
                {.samples = 50, .opsPerSample = n},
                [&] {
                    sig = std::make_unique<duin::Signal<int>>();
                    conns.clear();
                    conns.reserve(n);
                },
                [&] {
                    for (size_t i = 0; i < n; ++i)
                        conns.push_back(sig->ConnectScoped([&counter](int v) { counter += v; }));
                },
                [&] {
                    CHECK(sig->GetListenerCount() == n);
                    conns.clear();
                    sig.reset();
                });
        }
    }

    TEST_CASE("Signal benchmark: emit")
    {
        auto &log = GetSignalBenchmarkLog();
        log.Section("Emit");
        log.Note("ns/item = time per listener invocation");

        for (size_t n : kEmitListenerCounts)
        {
            size_t emits = EmitsFor(n);
            duin::Signal<int> sig;
            std::int64_t counter = 0;
            for (size_t i = 0; i < n; ++i)
                sig.Connect([&counter](int v) { counter += v; });

            RunAndRecord(
                std::format("Emit<int> listeners={}", n),
                {.samples = 30, .opsPerSample = emits, .itemsPerOp = std::max<size_t>(n, 1)},
                nullptr,
                [&] {
                    for (size_t e = 0; e < emits; ++e)
                        sig.Emit(static_cast<int>(e));
                    Consume(counter);
                });
        }

        for (size_t n : kEmitListenerCounts)
        {
            size_t emits = EmitsFor(n);
            std::vector<std::function<void(int)>> raw;
            std::int64_t counter = 0;
            for (size_t i = 0; i < n; ++i)
                raw.emplace_back([&counter](int v) { counter += v; });

            RunAndRecord(
                std::format("Baseline vector<function> listeners={}", n),
                {.samples = 30, .opsPerSample = emits, .itemsPerOp = std::max<size_t>(n, 1)},
                nullptr,
                [&] {
                    for (size_t e = 0; e < emits; ++e)
                        for (auto &fn : raw)
                            fn(static_cast<int>(e));
                    Consume(counter);
                });
        }

        for (size_t n : kListenerCounts)
        {
            size_t emits = EmitsFor(n);
            duin::Signal<int> sig;
            std::vector<std::shared_ptr<duin::ScopedConnection>> conns;
            std::int64_t counter = 0;
            for (size_t i = 0; i < n; ++i)
                conns.push_back(sig.ConnectScoped([&counter](int v) { counter += v; }));

            RunAndRecord(
                std::format("Emit<int> scoped listeners={}", n),
                {.samples = 30, .opsPerSample = emits, .itemsPerOp = n},
                nullptr,
                [&] {
                    for (size_t e = 0; e < emits; ++e)
                        sig.Emit(static_cast<int>(e));
                    Consume(counter);
                });
        }
    }

    TEST_CASE("Signal benchmark: emit argument types")
    {
        auto &log = GetSignalBenchmarkLog();
        log.Section("Emit argument types (100 listeners)");

        constexpr size_t n = 100;
        size_t emits = EmitsFor(n);
        const std::string payload(64, 'x');

        {
            duin::Signal<std::string> sig;
            std::int64_t counter = 0;
            for (size_t i = 0; i < n; ++i)
                sig.Connect([&counter](std::string s) { counter += static_cast<std::int64_t>(s.size()); });

            RunAndRecord(
                "Emit<std::string> (by value, 64 chars)",
                {.samples = 30, .opsPerSample = emits, .itemsPerOp = n},
                nullptr,
                [&] {
                    for (size_t e = 0; e < emits; ++e)
                        sig.Emit(payload);
                    Consume(counter);
                });
        }

        {
            duin::Signal<const std::string &> sig;
            std::int64_t counter = 0;
            for (size_t i = 0; i < n; ++i)
                sig.Connect([&counter](const std::string &s) { counter += static_cast<std::int64_t>(s.size()); });

            RunAndRecord(
                "Emit<const std::string&> (64 chars)",
                {.samples = 30, .opsPerSample = emits, .itemsPerOp = n},
                nullptr,
                [&] {
                    for (size_t e = 0; e < emits; ++e)
                        sig.Emit(payload);
                    Consume(counter);
                });
        }

        {
            duin::Signal<LargePayload> sig;
            LargePayload big;
            big.values.fill(1);
            std::int64_t counter = 0;
            for (size_t i = 0; i < n; ++i)
                sig.Connect([&counter](LargePayload p) { counter += p.values[0]; });

            RunAndRecord(
                "Emit<LargePayload> (128 bytes, by value)",
                {.samples = 30, .opsPerSample = emits, .itemsPerOp = n},
                nullptr,
                [&] {
                    for (size_t e = 0; e < emits; ++e)
                        sig.Emit(big);
                    Consume(counter);
                });
        }

        {
            duin::Signal<int, float, double> sig;
            double counter = 0;
            for (size_t i = 0; i < n; ++i)
                sig.Connect([&counter](int a, float b, double c) { counter += a + b + c; });

            RunAndRecord(
                "Emit<int, float, double>", {.samples = 30, .opsPerSample = emits, .itemsPerOp = n}, nullptr, [&] {
                    for (size_t e = 0; e < emits; ++e)
                        sig.Emit(static_cast<int>(e), 1.0f, 2.0);
                    Consume(static_cast<std::int64_t>(counter));
                });
        }
    }

    TEST_CASE("Signal benchmark: disconnect")
    {
        auto &log = GetSignalBenchmarkLog();
        log.Section("Disconnect");

        for (size_t n : kListenerCounts)
        {
            std::unique_ptr<duin::Signal<int>> sig;
            std::vector<duin::UUID> ids;
            RunAndRecord(
                std::format("Disconnect(UUID) front-to-back x{}", n),
                {.samples = 30, .opsPerSample = n},
                [&] {
                    sig = std::make_unique<duin::Signal<int>>();
                    ids.clear();
                    for (size_t i = 0; i < n; ++i)
                        ids.push_back(sig->Connect([](int) {}));
                },
                [&] {
                    for (auto &id : ids)
                        sig->Disconnect(id);
                },
                [&] {
                    CHECK(sig->GetListenerCount() == 0);
                    sig.reset();
                });
        }

        for (size_t n : kListenerCounts)
        {
            std::unique_ptr<duin::Signal<int>> sig;
            std::vector<duin::UUID> ids;
            RunAndRecord(
                std::format("Disconnect(UUID) back-to-front x{}", n),
                {.samples = 30, .opsPerSample = n},
                [&] {
                    sig = std::make_unique<duin::Signal<int>>();
                    ids.clear();
                    for (size_t i = 0; i < n; ++i)
                        ids.push_back(sig->Connect([](int) {}));
                },
                [&] {
                    for (auto it = ids.rbegin(); it != ids.rend(); ++it)
                        sig->Disconnect(*it);
                },
                [&] {
                    CHECK(sig->GetListenerCount() == 0);
                    sig.reset();
                });
        }

        for (size_t n : kListenerCounts)
        {
            std::unique_ptr<duin::Signal<int>> sig;
            std::vector<std::shared_ptr<duin::ScopedConnection>> conns;
            RunAndRecord(
                std::format("ScopedConnection release (shared_ptr reset) x{}", n),
                {.samples = 30, .opsPerSample = n},
                [&] {
                    sig = std::make_unique<duin::Signal<int>>();
                    conns.clear();
                    for (size_t i = 0; i < n; ++i)
                        conns.push_back(sig->ConnectScoped([](int) {}));
                },
                [&] { conns.clear(); },
                [&] {
                    CHECK(sig->GetListenerCount() == 0);
                    sig.reset();
                });
        }

        for (size_t n : kListenerCounts)
        {
            std::unique_ptr<duin::Signal<int>> sig;
            RunAndRecord(
                std::format("DisconnectAll listeners={}", n),
                {.samples = 30, .opsPerSample = 1, .itemsPerOp = n},
                [&] {
                    sig = std::make_unique<duin::Signal<int>>();
                    for (size_t i = 0; i < n; ++i)
                        sig->Connect([](int) {});
                },
                [&] { sig->DisconnectAll(); },
                [&] {
                    CHECK(sig->GetListenerCount() == 0);
                    sig.reset();
                });
        }

        for (size_t n : kListenerCounts)
        {
            std::unique_ptr<duin::Signal<int>> sig;
            std::vector<std::shared_ptr<duin::ScopedConnection>> conns;
            RunAndRecord(
                std::format("Signal destroy with scoped listeners={}", n),
                {.samples = 30, .opsPerSample = 1, .itemsPerOp = n},
                [&] {
                    sig = std::make_unique<duin::Signal<int>>();
                    conns.clear();
                    for (size_t i = 0; i < n; ++i)
                        conns.push_back(sig->ConnectScoped([](int) {}));
                },
                [&] { sig.reset(); },
                [&] { conns.clear(); });
        }
    }

    TEST_CASE("Signal benchmark: disconnect during emit")
    {
        auto &log = GetSignalBenchmarkLog();
        log.Section("Disconnect during emit");
        log.Note("Each listener disconnects itself while the signal is emitting (deferred erase queue)");

        for (size_t n : kListenerCounts)
        {
            std::unique_ptr<duin::Signal<int>> sig;
            std::vector<duin::UUID> ids;
            RunAndRecord(
                std::format("Emit with self-disconnect listeners={}", n),
                {.samples = 30, .opsPerSample = 1, .itemsPerOp = n},
                [&] {
                    sig = std::make_unique<duin::Signal<int>>();
                    ids.assign(n, duin::UUID::INVALID);
                    for (size_t i = 0; i < n; ++i)
                    {
                        duin::Signal<int> *raw = sig.get();
                        ids[i] = sig->Connect([raw, &ids, i](int) { raw->Disconnect(ids[i]); });
                    }
                },
                [&] { sig->Emit(0); },
                [&] {
                    CHECK(sig->GetListenerCount() == 0);
                    sig.reset();
                });
        }

        for (size_t n : kListenerCounts)
        {
            std::unique_ptr<duin::Signal<int>> sig;
            RunAndRecord(
                std::format("Emit with DisconnectAll in first listener, listeners={}", n),
                {.samples = 30, .opsPerSample = 1, .itemsPerOp = n},
                [&] {
                    sig = std::make_unique<duin::Signal<int>>();
                    duin::Signal<int> *raw = sig.get();
                    sig->Connect([raw](int) { raw->DisconnectAll(); });
                    for (size_t i = 1; i < n; ++i)
                        sig->Connect([](int) {});
                },
                [&] { sig->Emit(0); },
                [&] {
                    CHECK(sig->GetListenerCount() == 0);
                    sig.reset();
                });
        }
    }
}
