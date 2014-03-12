cmake_minimum_required(VERSION 2.8)

set(CMAKE_C_COMPILER clang)
set(CMAKE_CXX_COMPILER clang++)

set(CMAKE_CXX_FLAGS "--std=c++11 -Weverything -Werror -Wno-c++98-compat -Wno-missing-prototypes -Wno-padded -fno-exceptions -Wno-old-style-cast")


set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -DSGL_DEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O3 -g -DNDEBUG")

################################################################################

set(sources sgl_test.cpp)

add_executable(test ${sources})
