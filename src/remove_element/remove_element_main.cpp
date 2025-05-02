#include <benchmark/benchmark.h>
// #include <boost/range/irange.hpp>
#include <boost/range/irange.hpp>

#include <algorithm>
#include <iterator>
#include <random>
#include <ranges>
#include <string_view>
#include <unordered_set>
#include <vector>

namespace
{
namespace others
{
int removeElement(std::vector<int> &nums, int val)
{
    auto right = std::ssize(nums) - 1;

    for (decltype(right) i = 0; i < right; i++)
    {
        if (nums[i] == val)
        {
            while (right > i && nums[right] == val)
            {
                right--;
            }

            std::swap(nums[i], nums[right]);
            right--;
        }
    }

    while (right >= 0 && nums[right] == val)
    {
        right--;
    }

    nums.resize(right + 1);
    return static_cast<int>(nums.size());
}
} // namespace others
namespace my
{
int removeElement(std::vector<int> &nums, int val)
{
    nums.erase(std::remove_if(nums.begin(), nums.end(), [val](auto const element) -> bool { return element == val; }),
               nums.end());
    return nums.size();
}
} // namespace my
namespace check
{
struct DataSet
{
    std::vector<int> nums;
    int to_remove;
    std::vector<int> expected;
};

std::vector<DataSet> make_dataset()
{
    std::vector<DataSet> dataset{
        DataSet{{}, 3, {}},
        DataSet{{3, 2, 2, 3}, 3, {2, 2}},
        DataSet{{0, 1, 2, 2, 3, 0, 4, 2}, 2, {0, 1, 3, 0, 4}},
        DataSet{{0, 1, 2, 2, 3, 0, 4, 2}, 9, {0, 1, 2, 2, 3, 0, 4, 2}},
    };

    std::mt19937 rnd(42);
    std::uniform_int_distribution<std::int32_t> values_inside(-100, +100);
    std::uniform_int_distribution<std::int32_t> nums_size(1, +100);
    auto const n_testcases = 1'000;
    for (auto const _ : boost::irange(n_testcases))
    {
        std::vector<int> nums(nums_size(rnd));
        std::generate(nums.begin(), nums.end(), [&]() { return values_inside(rnd); });
        std::uniform_int_distribution<std::size_t> rnd_element(0, nums.size() - 1);
        auto const to_remove = nums[rnd_element(rnd)];
        std::vector<int> expected;
        expected.reserve(nums.size());
        std::copy_if(nums.begin(), nums.end(), std::back_inserter(expected),
                     [&to_remove](auto const &element) { return to_remove != element; });
        dataset.push_back(DataSet{nums, to_remove, expected});
    }

    return dataset;
}

template <size_t N> struct StringLiteral
{
    constexpr StringLiteral(const char (&str)[N])
    {
        std::copy_n(str, N, value);
    }

    char value[N];

    constexpr std::string_view as_sv() const
    {
        return std::string_view{value, value + sizeof(value) / sizeof(value[0]) - 1};
    }
};

void check_if_valid(int size_after_remove, std::vector<int> nums, DataSet const &ds, std::string_view name)
{
    namespace ranges = std::ranges;
    auto const make_msg = [&](auto prefix_msg) {
        auto delimter = "";
        std::string nums_as_str;
        for (auto const &n : nums)
        {
            (nums_as_str += delimter) += std::to_string(n);
            delimter = ",";
        }
        delimter = "";
        std::string expect_as_str;
        for (auto const &n : ds.expected)
        {
            (expect_as_str += delimter) += std::to_string(n);
            delimter = ",";
        }

        return std::format("in \"{}\" {} nums:[{}] expect:[{}]", name, prefix_msg, nums_as_str, expect_as_str);
    };
    if (size_after_remove != static_cast<int>(nums.size()))
        throw std::runtime_error(make_msg(std::format("return value:{} has not the same size than nums.size() -> {}",
                                                      size_after_remove, ds.nums.size())));
    if (ranges::to<std::unordered_set<int>>(nums) != ranges::to<std::unordered_set<int>>(ds.expected))
        throw std::runtime_error(make_msg("nums and expected differ"));
}

} // namespace check
} // namespace

template <auto &fnptr, check::StringLiteral lit> static void bench(benchmark::State &state)
{
    auto dataset = check::make_dataset();
    for (auto _ : state)
    {
        for (auto &ds : dataset)
        {
            auto nums = ds.nums;
            auto size_after_remove = fnptr(nums, ds.to_remove);
            check::check_if_valid(size_after_remove, nums, ds, lit.as_sv());
            benchmark::DoNotOptimize(size_after_remove);
        }
    }
}

BENCHMARK_TEMPLATE2(bench, others::removeElement, "others::removeElement");
BENCHMARK_TEMPLATE2(bench, my::removeElement, "my::removeElement");

BENCHMARK_MAIN();