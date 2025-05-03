#include <benchmark/benchmark.h>
#include <filesystem>
#include <memory>
#include <vector>

namespace bench_utils
{

struct BenchArgs
{
    std::vector<char *> argv;
    int argc;
    std::filesystem::path vega_plot_path;
    std::optional<std::tuple<int, int>> range;
};

BenchArgs extract_args(int argc, char **argv);

class MyReporter : public benchmark::BenchmarkReporter
{
  public:
    void addReporter(std::unique_ptr<benchmark::BenchmarkReporter> &&rep);
    bool ReportContext(const Context &context) override;
    void ReportRuns(const std::vector<Run> &reports) override;
    void Finalize() override;
    std::vector<std::unique_ptr<benchmark::BenchmarkReporter>> &GetReporter();

  private:
    std::vector<std::unique_ptr<benchmark::BenchmarkReporter>> reporter_;
};

void make_vega_plot(std::string_view benchmark_report_as_json, std::ostream &out);

std::unique_ptr<MyReporter> make_reporter(std::ostream &out);

struct Benchmark
{
    void (*bench_f)(benchmark::State &);
    std::string name;
};

int benchmark_main(std::vector<Benchmark> &benchmarks, int argc, char **argv);
} // namespace bench_utils

#define REG_BENCH(benchf)                                                                                              \
    bench_utils::Benchmark                                                                                             \
    {                                                                                                                  \
        (benchf), (#benchf)                                                                                            \
    }
