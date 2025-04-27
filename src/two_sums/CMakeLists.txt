add_executable(two_sums_benchmark main.cpp)

set_property(TARGET two_sums_benchmark PROPERTY CXX_STANDARD 23)
target_link_libraries(two_sums_benchmark benchmark::benchmark)
