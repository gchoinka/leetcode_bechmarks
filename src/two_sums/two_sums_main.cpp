#include "boost/container/container_fwd.hpp"
#include "boost/json/parser.hpp"
#include <algorithm>
#include <benchmark/benchmark.h>
#include <boost/container/small_vector.hpp>
#include <boost/container/static_vector.hpp>
#include <boost/filesystem.hpp>
#include <boost/json.hpp>

#include <cstddef>
#include <exception>
#include <format>
#include <limits>
#include <map>
#include <memory>
#include <print>
#include <random>
#include <sstream>
#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>

namespace
{
namespace others
{
std::vector<int> twoSum_unordered_map(std::vector<int> const &nums, int target)
{
    std::unordered_map<int, int> hello;
    for (int i = 0; i < static_cast<int>(nums.size()); i++)
    {
        if (hello.find(target - nums[i]) != hello.end())
        {
            return {hello[target - nums[i]] - 1, i};
        }
        hello[nums[i]] = i + 1;
    }
    return {};
}

std::vector<int> twoSum_map(std::vector<int> const &nums, int target)
{
    std::map<int, int> num;
    for (int i = 0; i < static_cast<int>(nums.size()); i++)
    {
        if (num.find((target - nums[i])) != num.end())
        {
            return {num[target - nums[i]], i};
        }
        num[nums[i]] = i;
    }
    return {};
}

std::vector<int> twoSum_n_square(std::vector<int> const &nums, int target)
{
    const int sizeOfNums = nums.size();

    for (int x = 0; x < sizeOfNums - 1; x++)
    {
        const int diff = target - nums[x];
        for (int y = x + 1; y < sizeOfNums; y++)
        {
            if (diff == nums[y])
            {
                return {x, y};
            }
        }
    }

    return {};
}
} // namespace others
namespace my
{
std::vector<int> twoSum_with_sort_vector(std::vector<int> const &nums, int target)
{
    std::vector<int> idx(nums.size());
    for (int i = 0; i < static_cast<int>(idx.size()); ++i)
        idx[i] = i;
    std::sort(idx.begin(), idx.end(), [&nums](auto const lhs, auto const rhs) { return nums[lhs] < nums[rhs]; });

    int f = 0, s = idx.size() - 1;
    while (f < static_cast<int>(idx.size()) && s >= 0 && f != s)
    {
        auto const candidate = nums[idx[f]] + nums[idx[s]];
        if (candidate == target)
        {
            return {idx[f], idx[s]};
        }
        else if (candidate > target)
        {
            --s;
        }
        else
        {
            ++f;
        }
    }

    return {};
}

std::vector<int> twoSum_with_sort_array(std::vector<int> const &nums, int target)
{
    constexpr std::size_t buffer_size = 2000;
    if (nums.size() > buffer_size)
    {
        return twoSum_with_sort_vector(nums, target);
    }
    std::array<int, buffer_size> idx;
    std::generate(idx.begin(), idx.begin() + nums.size(), [i = 0]() mutable { return i++; });
    std::sort(idx.begin(), idx.begin() + nums.size(),
              [&nums](auto const lhs, auto const rhs) { return nums[lhs] < nums[rhs]; });

    int f = 0, s = nums.size() - 1;
    while (f < static_cast<int>(nums.size()) && s >= 0 && f != s)
    {
        auto const candidate = nums[idx[f]] + nums[idx[s]];
        if (candidate == target)
        {
            return {idx[f], idx[s]};
        }
        else if (candidate > target)
        {
            --s;
        }
        else
        {
            ++f;
        }
    }

    return {};
}

} // namespace my

namespace check
{
struct TestSet
{
    std::vector<int> nums;
    int target;
};

std::vector<TestSet> make_dataset(std::size_t num_max_size = 2'000)
{
    // std::mt19937 generator((std::random_device{})());
    std::mt19937 generator(42);
    std::uniform_int_distribution<std::int32_t> distribute(std::numeric_limits<std::int32_t>::min() / 2 + 1,
                                                           std::numeric_limits<std::int32_t>::max() / 2 - 1);

    auto test_cases = std::vector<TestSet>{
        TestSet{{1, 2, 3, 6}, 8}, TestSet{{2, 7, 11, 15}, 9}, TestSet{{3, 2, 4}, 6},
        TestSet{{3, 2, 4}, 5},    TestSet{{3, 2, 4}, 7},      TestSet{{3, 3}, 6},
    };

    std::uniform_int_distribution<std::int32_t> nums_size_dist(2, num_max_size);
    const auto n_test_cases = 1e4;

    auto gen = [&]() { return distribute(generator); };

    for (int i = 0; i < n_test_cases; ++i)
    {
        auto const nums_size = nums_size_dist(generator);

        std::vector<int> nums(nums_size);
        std::generate(nums.begin(), nums.end(), gen);
        std::sort(nums.begin(), nums.end());
        auto const end_of_unique = std::unique(nums.begin(), nums.end());
        nums.resize(std::distance(nums.begin(), end_of_unique));
        if (nums.size() < 2)
            continue;
        std::uniform_int_distribution<std::int32_t> choose_two(0, nums.size() - 1);
        auto const first = choose_two(generator);
        auto const second = choose_two(generator);
        if (first == second)
            continue;
        auto const target = nums[first] + nums[second];

        std::shuffle(nums.begin(), nums.end(), generator);
        test_cases.push_back(TestSet{std::move(nums), target});
    }

    return test_cases;
}

void check_if_valid(std::vector<int> const &nums, std::vector<int> const &two_idx, int target)
{
    auto print_testset = [&]() {
        std::string nums_as_str;
        char const *delimiter = "";
        for (auto const n : nums)
        {
            (nums_as_str += delimiter) += std::to_string(n);
            delimiter = ",";
        }
        std::string ids_as_str;
        if (two_idx.size() > 0)
            ids_as_str += std::to_string(two_idx[0]);
        if (two_idx.size() > 1)
            (ids_as_str += ",") += std::to_string(two_idx[1]);

        return std::format("two_idx:[{}] target:{} nums:[{}] ", ids_as_str, target, nums_as_str);
    };
    if (two_idx.size() != 2)
        throw std::runtime_error(
            std::format("(two_idx.size() -> {}) != 2 ; testset:{}", two_idx.size(), print_testset()));
    for (auto const i : std::vector{0, 1})
    {
        if (two_idx[i] >= static_cast<int>(nums.size()))
            throw std::runtime_error(std::format("(two_idx[{}] -> {}) >= (nums.size() -> {}) ; testset:{}", i,
                                                 two_idx[i], nums.size(), print_testset()));
    }

    if ((nums[two_idx[0]] + nums[two_idx[1]]) != target)
        throw std::runtime_error(std::format("target is not as expected; testset:{}", print_testset()));
}
} // namespace check

template <auto &fnptr> static void bench(benchmark::State &state)
{
    auto const dataset = check::make_dataset(state.range(0));
    for (auto _ : state)
    {
        for (auto const &test_set : dataset)
        {
            auto b = fnptr(test_set.nums, test_set.target);
            check::check_if_valid(test_set.nums, b, test_set.target);
            benchmark::DoNotOptimize(b);
        }
    }
    state.counters["range"] = state.range(0);
}

struct Benchmark
{
    void (*bench_f)(benchmark::State &);
    std::string name;
};
#define REG_BENCH(benchf)                                                                                              \
    Benchmark                                                                                                          \
    {                                                                                                                  \
        (benchf), (#benchf)                                                                                            \
    }

} // namespace

class MyReporter : public benchmark::BenchmarkReporter
{
  public:
    MyReporter()
    {
    }

    void addReporter(std::unique_ptr<benchmark::BenchmarkReporter> &&rep)
    {
        reporter_.push_back(std::move(rep));
    }
    bool ReportContext(const Context &context) override
    {
        bool results = true;
        for (auto &r : reporter_)
            results = results && r->ReportContext(context);
        return results;
    }
    void ReportRuns(const std::vector<Run> &reports) override
    {
        for (auto &r : reporter_)
            r->ReportRuns(reports);
    }
    void Finalize() override
    {
        for (auto &r : reporter_)
            r->Finalize();
    }

    std::vector<std::unique_ptr<benchmark::BenchmarkReporter>> &GetReporter()
    {
        return reporter_;
    }

  private:
    std::vector<std::unique_ptr<benchmark::BenchmarkReporter>> reporter_;
};

int main(int argc, char **argv)
{
    constexpr auto range_start = 8;
    constexpr auto range_end = 8 << 2;

    std::vector<Benchmark> benches{REG_BENCH(bench<others::twoSum_unordered_map>), REG_BENCH(bench<others::twoSum_map>),
                                   REG_BENCH(bench<others::twoSum_n_square>),
                                   REG_BENCH(bench<my::twoSum_with_sort_vector>),
                                   REG_BENCH(bench<my::twoSum_with_sort_array>)};
    benches.resize(2);
    for (auto const &b : benches)
        benchmark::RegisterBenchmark(b.name, b.bench_f)->Range(range_start, range_end);

    benchmark::Initialize(&argc, argv);
    auto reporter = std::make_unique<MyReporter>();
    reporter->addReporter(std::unique_ptr<benchmark::BenchmarkReporter>{benchmark::CreateDefaultDisplayReporter()});
    reporter->addReporter(std::make_unique<benchmark::JSONReporter>());
    std::stringstream outstream;
    reporter->GetReporter().back()->SetOutputStream(&outstream);
    benchmark::RunSpecifiedBenchmarks(reporter.release());
    std::print("{}", outstream.str());

    benchmark::Shutdown();

    auto report = boost::json::parse(outstream.str());
    for( auto const & r: report.at("benchmarks").as_array())
    {
        std::print("names {}\n", r.at("name").as_string().c_str());
    }
}

#undef REG_BENCH