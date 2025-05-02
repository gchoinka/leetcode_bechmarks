#include <benchmark/benchmark.h>
#include <filesystem>
#include <memory>
#include <vector>

namespace bench_utils
{

struct FilteredArgv
{
    std::vector<char *> argv;
    int argc;
    std::filesystem::path vega_plot_path;
    int range_start = 8 << 1;
    int range_end = 8 << 10;
};

FilteredArgv filter(int argc, char **argv);

class MyReporter : public benchmark::BenchmarkReporter
{
  public:
    MyReporter();

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
} // namespace bench_utils