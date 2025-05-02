

add_library(bench_utils STATIC "${CMAKE_CURRENT_SOURCE_DIR}/common/bench_utils.cpp")
target_include_directories(bench_utils PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/common/include")

target_link_libraries(bench_utils PRIVATE benchmark::benchmark)
target_link_libraries(bench_utils PRIVATE Boost::json)
target_link_libraries(bench_utils PRIVATE Boost::filesystem)
target_link_libraries(bench_utils PRIVATE Boost::regex)

add_subdirectory(reverse_int32)
add_subdirectory(two_sums)
add_subdirectory(sandbox)
add_subdirectory(remove_element)