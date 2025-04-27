#include <benchmark/benchmark.h>

#include <array>
#include <cstdint>
#include <format>
#include <limits>
#include <random>
namespace
{
namespace others
{

std::int32_t reverse_modulo_ten(std::int32_t x)
{
    std::uint64_t ret = 0;
    std::int64_t sign = x >= 0 ? 1 : -1;
    std::uint64_t px = x * sign;
    while (px != 0)
    {
        std::uint64_t m = px / 10;
        std::uint64_t d = px % 10;
        if (d != 0)
            ret += d;
        if (ret > std::numeric_limits<std::int32_t>::max())
            return 0;
        if (m != 0)
            ret *= 10;
        if (ret > std::numeric_limits<std::int32_t>::max())
            return 0;
        px = m;
    }
    return sign * ret;
}
} // namespace others
namespace my
{
constexpr bool check_will_overflow(std::int32_t x)
{
    if (x > 0 && x < 1e9)
        return false;

    if (x < 0 && x > -1e9)
        return false;

    std::uint8_t last_digit = 7;
    if (x == -2147483648)
    {
        return true;
    }
    if (x < 0)
    {
        x *= -1;
        last_digit = 8;
    }

    std::array<std::uint8_t, 10> int_max{2, 1, 4, 7, 4, 8, 3, 6, 4, last_digit};
    for (auto const v : int_max)
    {
        auto const to_check = x % 10;
        x = x / 10;
        if (to_check == v)
            continue;
        return to_check > v;
    }
    return false;
}

constexpr auto lookup = []() {
    std::array<std::uint8_t, 100> v;
    for (int i = 0; i < 100; ++i)
    {
        v[i] = (i % 10) * 10 + i / 10;
    }
    return v;
}();

std::int32_t reverse_int32(std::int32_t x)
{
    if (check_will_overflow(x))
        return 0;
    auto const sign = x >= 0 ? 1 : -1;
    x *= sign;

    if (x < 10)
    {
        return x * sign;
    }

    std::int32_t result = 0;
    while (x >= 10)
    {
        result *= 100;
        auto last_d = x % 100;
        x = x / 100;
        result += lookup[last_d];
    }
    if (x != 0)
        result *= 10;
    return (result + x) * sign;
}
} // namespace my
namespace check
{
static auto const random_num = []() {
    std::mt19937 generator((std::random_device{})());
    std::uniform_int_distribution<std::int32_t> distribute(std::numeric_limits<std::int32_t>::min(),
                                                           std::numeric_limits<std::int32_t>::max());
    std::vector<std::int32_t> test_numbers;
    auto const n = 1e6;
    test_numbers.reserve(n);
    for (int i = 0; i < n; ++i)
    {
        test_numbers.push_back(distribute(generator));
    }
    return test_numbers;
}();
} // namespace check
} // namespace
static void bench_others_reverse_modulo_ten(benchmark::State &state)
{
    for (auto _ : state)
    {
        for (auto const & num : check::random_num)
        {
            auto b = others::reverse_modulo_ten(num);
            benchmark::DoNotOptimize(b);
        }
    }
}
// Register the function as a benchmark
BENCHMARK(bench_others_reverse_modulo_ten);

static void bench_my_reverse_int32(benchmark::State &state)
{
    for (auto _ : state)
    {
        for (auto const & num : check::random_num)
        {
            auto b = my::reverse_int32(num);
            benchmark::DoNotOptimize(b);
        }
    }
}
BENCHMARK(bench_my_reverse_int32);

static void bench_reverse_check_same(benchmark::State &state)
{
    for (auto _ : state)
    {
        for (auto const & num : check::random_num)
        {
            auto b1 = my::reverse_int32(num);
            auto b2 = others::reverse_modulo_ten(num);
            if (b1 != b2)
                throw std::runtime_error(
                    std::format("Found missmatch for number:{} my::reverse_int32:{} others::reverse_modulo_ten:{}", num, b1, b2));
            benchmark::DoNotOptimize(b1);
        }
    }
}
BENCHMARK(bench_reverse_check_same);

BENCHMARK_MAIN();