#include <benchmark/benchmark.h>

#include <exception>
#include <format>
#include <iostream>
#include <map>
#include <random>
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
std::vector<int> twoSum_with_sort(std::vector<int> const &nums, int target)
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
} // namespace my

namespace check
{
struct TestSet
{
    std::vector<int> nums;
    int target;
};

std::vector<TestSet> const test_sets = []() {
    // std::mt19937 generator((std::random_device{})());
    std::mt19937 generator(42);
    std::uniform_int_distribution<std::int32_t> distribute(std::numeric_limits<std::int32_t>::min() / 2 + 1,
                                                           std::numeric_limits<std::int32_t>::max() / 2 - 1);

    auto test_cases = std::vector<TestSet>{
        TestSet{{1, 2, 3, 6}, 8}, TestSet{{2, 7, 11, 15}, 9}, TestSet{{3, 2, 4}, 6},
        TestSet{{3, 2, 4}, 5},    TestSet{{3, 2, 4}, 7},      TestSet{{3, 3}, 6},
    };

    std::uniform_int_distribution<std::int32_t> nums_size_dist(2, 2'000);
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
}();

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
} // namespace

static void bench_others_twoSum_unordered_map(benchmark::State &state)
{
    for (auto _ : state)
    {
        for (auto const &test_set : check::test_sets)
        {
            auto b = others::twoSum_unordered_map(test_set.nums, test_set.target);
            check::check_if_valid(test_set.nums, b, test_set.target);
            benchmark::DoNotOptimize(b);
        }
    }
}
BENCHMARK(bench_others_twoSum_unordered_map);

static void bench_others_twoSum_map(benchmark::State &state)
{
    for (auto _ : state)
    {
        for (auto const &test_set : check::test_sets)
        {
            auto b = others::twoSum_map(test_set.nums, test_set.target);
            check::check_if_valid(test_set.nums, b, test_set.target);
            benchmark::DoNotOptimize(b);
        }
    }
}
BENCHMARK(bench_others_twoSum_map);

static void bench_others_twoSum_n_square(benchmark::State &state)
{
    for (auto _ : state)
    {
        for (auto const &test_set : check::test_sets)
        {
            auto b = others::twoSum_n_square(test_set.nums, test_set.target);
            check::check_if_valid(test_set.nums, b, test_set.target);
            benchmark::DoNotOptimize(b);
        }
    }
}
BENCHMARK(bench_others_twoSum_n_square);

static void bench_my_twoSum_with_sort(benchmark::State &state)
{
    for (auto _ : state)
    {
        for (auto const & test_set : check::test_sets)
        {
            auto b = my::twoSum_with_sort(test_set.nums, test_set.target);
            check::check_if_valid(test_set.nums, b, test_set.target);
            benchmark::DoNotOptimize(b);
        }
    }
}
BENCHMARK(bench_my_twoSum_with_sort);

BENCHMARK_MAIN();