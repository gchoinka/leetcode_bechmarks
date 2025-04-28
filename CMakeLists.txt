cmake_minimum_required(VERSION 3.28)


project(Playground LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_compile_options(
    $<$<CXX_COMPILER_ID:GNU>:-fdiagnostics-color=always>
    $<$<CXX_COMPILER_ID:Clang>:-fcolor-diagnostics>
)


file(
  DOWNLOAD
  https://github.com/cpm-cmake/CPM.cmake/releases/download/v0.39.0/CPM.cmake
  ${CMAKE_CURRENT_BINARY_DIR}/cmake/CPM.cmake
  EXPECTED_HASH SHA256=66639BCAC9DD2907B2918DE466783554C1334446B9874E90D38E3778D404C2EF
)
include(${CMAKE_CURRENT_BINARY_DIR}/cmake/CPM.cmake)


CPMAddPackage(
  NAME benchmark
  GITHUB_REPOSITORY google/benchmark
  VERSION 1.9.2
  OPTIONS "BENCHMARK_ENABLE_TESTING Off"
)

add_compile_options(-Wall -Wextra -Wpedantic)

# add_custom_target(
#     link_compile_commands_to_root ALL
#     DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/__compile_commands.json
# )

# add_custom_command(
#     OUTPUT
#         ${CMAKE_CURRENT_BINARY_DIR}/__compile_commands.json
#         ${CMAKE_CURRENT_SOURCE_DIR}/compile_commands.json  
#     COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_CURRENT_BINARY_DIR}/compile_commands.json ${CMAKE_CURRENT_SOURCE_DIR}/compile_commands.json  
# )

add_subdirectory(src)