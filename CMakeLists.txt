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



CPMAddPackage(
  NAME Boost
  VERSION 1.88.0 # Versions less than 1.85.0 may need patches for installation targets.
  URL https://github.com/boostorg/boost/releases/download/boost-1.88.0/boost-1.88.0-cmake.tar.xz
  URL_HASH SHA256=f48b48390380cfb94a629872346e3a81370dc498896f16019ade727ab72eb1ec
  OPTIONS "BOOST_ENABLE_CMAKE ON" "BOOST_SKIP_INSTALL_RULES ON" # Set `OFF` for installation
  "BUILD_SHARED_LIBS ON" "BOOST_INCLUDE_LIBRARIES container\\\;range\\\;json\\\\;filesystem" # Note the escapes!
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
