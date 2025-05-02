# collection of leetcode questions

my solution vs others testes with google benchmark (c), Leetcode’s performance measuring is too coarse and all the solutions end up in the 0ms bracket.

## twoSum

```mermaid
graph TD
    A[Start] --> B[Create index array idx]
    B --> C[Sort index array idx based on nums values]
    C --> D[Initialize pointers: f=0, s=size-1]
    D --> E{Check if f and s pointers valid}
    E -->|Yes| F["Calculate sum between smalles nums[idx[f]] and biggest nums[idx[s]] cadidate"]
    F --> G{sum == target?}
    G -->|Yes| H["Return idx[f], idx[s] indices"]
    G -->|No| I{sum > target?}
    I -->|Yes| J[Decrement s]
    I -->|No| K[Increment f]
    J --> E
    K --> E
    E -->|No| L[Return empty vector]
    H --> M[End]
    L --> M
```

![visualization](https://github.com/user-attachments/assets/f7f8a695-3248-4b9a-8730-58b7d563e7ee)


## reverse int 

my::reverse_int32 precalculated an lookup table for 2 digits so in theory it can eat the number up faster by doing modulo 100 and divide by 100 instead the trivial approach with 10.


```
Running ./build/src/reverse_int32/reverse_int32_benchmark
Run on (12 X 2592.01 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x6)
  L1 Instruction 32 KiB (x6)
  L2 Unified 256 KiB (x6)
  L3 Unified 9216 KiB (x1)
Load Average: 0.08, 0.12, 0.09
----------------------------------------------------------------------------
Benchmark                                  Time             CPU   Iterations
----------------------------------------------------------------------------
bench<others::reverse_modulo_ten>   22400127 ns     22343004 ns           27
bench<my::reverse_int32>            11750695 ns     11715982 ns           50
```
