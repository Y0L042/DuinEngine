#pragma once

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <functional>
#include <string>

namespace duin::bench
{

struct BenchmarkStats
{
    std::string name;
    std::size_t samples = 0;
    std::size_t opsPerSample = 0;
    std::size_t itemsPerOp = 1;
    double minNs = 0.0;
    double medianNs = 0.0;
    double meanNs = 0.0;
    double maxNs = 0.0;
    double stddevNs = 0.0;
};

struct BenchmarkConfig
{
    std::size_t samples = 30;
    std::size_t warmupSamples = 3;
    std::size_t opsPerSample = 1;
    std::size_t itemsPerOp = 1;
};

class BenchmarkLog
{
  public:
    BenchmarkLog(const std::filesystem::path &directory, const std::string &prefix);
    ~BenchmarkLog();

    BenchmarkLog(const BenchmarkLog &) = delete;
    BenchmarkLog &operator=(const BenchmarkLog &) = delete;

    void Section(const std::string &title);
    void Note(const std::string &text);
    void Record(const BenchmarkStats &stats);

    const std::filesystem::path &GetPath() const;

  private:
    std::filesystem::path path_;
    std::ofstream file_;
    std::chrono::steady_clock::time_point start_;

    void WriteHeader();
    void WriteTableHeader();
    void WriteLine(const std::string &line);
};

BenchmarkLog &GetSignalBenchmarkLog();

void Consume(std::int64_t value);

BenchmarkStats RunBenchmark(
    const std::string &name, const BenchmarkConfig &config, const std::function<void()> &setup,
    const std::function<void()> &body, const std::function<void()> &teardown = {});

BenchmarkStats RunAndRecord(
    const std::string &name, const BenchmarkConfig &config, const std::function<void()> &setup,
    const std::function<void()> &body, const std::function<void()> &teardown = {});

} // namespace duin::bench
