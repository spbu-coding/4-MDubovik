cmake_minimum_required(VERSION 3.7)
project(tests LANGUAGES C)

set(CMAKE_C_STANDARD 99)

file(GLOB_RECURSE converter_sources src/converter/*.c src/converter/*.h)

add_executable(converter ${converter_sources})
#add_executable(comparer )