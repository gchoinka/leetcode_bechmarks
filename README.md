# collection of leetcode questions

my solution vs others testes with google benchmark (c)

## twoSum

```
Running ./build/src/two_sums/two_sums_benchmark
Run on (12 X 2592.01 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x6)
  L1 Instruction 32 KiB (x6)
  L2 Unified 256 KiB (x6)
  L3 Unified 9216 KiB (x1)
Load Average: 0.22, 0.17, 0.11
----------------------------------------------------------------------------
Benchmark                                  Time             CPU   Iterations
----------------------------------------------------------------------------
bench_others_twoSum_unordered_map  792747768 ns    774914200 ns            1
bench_others_twoSum_map           1012166419 ns    989389800 ns            1
bench_others_twoSum_n_square       991108060 ns    968802100 ns            1
bench_my_twoSum_with_sort          436706361 ns    426882550 ns            2
```

## reverse int

```
Running ./build/src/reverse_int32/reverse_int32_benchmark
Run on (12 X 2592.01 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x6)
  L1 Instruction 32 KiB (x6)
  L2 Unified 256 KiB (x6)
  L3 Unified 9216 KiB (x1)
Load Average: 0.15, 0.08, 0.08
--------------------------------------------------------------------------
Benchmark                                Time             CPU   Iterations
--------------------------------------------------------------------------
bench_others_reverse_modulo_ten   24933462 ns     24370678 ns           23
bench_my_reverse_int32            11154018 ns     10901192 ns           52
bench_reverse_check_same          37418397 ns     36600529 ns           17
```