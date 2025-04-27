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
    if (x < 0) // if negaive make it positive
    {
        x *= -1;
        last_digit = 8;
    }

    std::array<std::uint8_t, 10> int32_max{2, 1, 4, 7, 4, 8, 3, 6, 4, last_digit};
    for (auto const v : int32_max)
    {
        auto const to_check = x % 10;
        x = x / 10;
        if (to_check == v)
            continue;
        return to_check > v;
    }
    return false;
}

constexpr auto swap_digits_lookup = []() {
    std::array<std::uint8_t, 100> v;
    for (int i = 0; i < 100; ++i)
    {
        v[i] = (i % 10) * 10 + i / 10;
    }
    return v;
}();

std::int32_t reverse_int32(std::int32_t x)
{
    if (check_will_overflow(x)) // only specific numbers will overflow, the idea here is that if we do the test before
                                // we can avoid doing the overflow test in the loop
        return 0;

    if (x < 10 && x > -10) // nothing to reverse here
        return x;

    auto const sign = x >= 0 ? 1 : -1;
    x *= sign; // make x positive, at the end we will add the sign again

    std::int32_t result = 0;
    while (x >= 10)
    {
        result *= 100;
        auto last_two_digits = x % 100;
        x = x / 100;
        result += swap_digits_lookup[last_two_digits];
    }
    if (x != 0)
        result *= 10;
    return (result + x) * sign;
}
} // namespace my
namespace check
{
std::vector<std::int32_t> make_dataset()
{
    std::mt19937 generator(42);
    std::uniform_int_distribution<std::int32_t> distribute(std::numeric_limits<std::int32_t>::min(),
                                                           std::numeric_limits<std::int32_t>::max());
    auto const n_testcases = 1e6;
    std::vector<std::int32_t> test_numbers(n_testcases);
    std::generate(test_numbers.begin(), test_numbers.end(), [&]() { return distribute(generator); });
    return test_numbers;
}

auto const _ = []() { // the test is run before main
    for (auto const &num : make_dataset())
    {
        auto b1 = my::reverse_int32(num);
        auto b2 = others::reverse_modulo_ten(num);
        if (b1 != b2)
            throw std::runtime_error(std::format(
                "Found missmatch for number:{} my::reverse_int32:{} others::reverse_modulo_ten:{}", num, b1, b2));
    }
    return true;
}();

} // namespace check
} // namespace

template <auto &fnptr> static void bench(benchmark::State &state)
{
    auto const dataset = check::make_dataset();
    for (auto _ : state)
    {
        for (auto const &num : dataset)
        {
            auto b = fnptr(num);
            benchmark::DoNotOptimize(b);
        }
    }
}

BENCHMARK_TEMPLATE1(bench, others::reverse_modulo_ten);
BENCHMARK_TEMPLATE1(bench, my::reverse_int32);

BENCHMARK_MAIN();