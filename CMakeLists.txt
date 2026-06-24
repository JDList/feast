cmake_minimum_required(VERSION 3.20)
project(FEAST VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(FEAST_BUILD_TESTS "Build tests" ON)
option(FEAST_BUILD_EXAMPLES "Build examples" ON)
option(FEAST_BUILD_PYTHON "Build Python bindings" ON)

add_subdirectory(src)
add_subdirectory(external/eigen)   # only if vendored as submodule
if(FEAST_BUILD_TESTS)
  add_subdirectory(tests)
endif()

if(FEAST_BUILD_EXAMPLES)
  add_subdirectory(examples)
endif()

if(FEAST_BUILD_PYTHON)
  add_subdirectory(python)
endif()
