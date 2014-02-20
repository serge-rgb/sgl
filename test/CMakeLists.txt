set(sources sgl_test.cpp)

set(CMAKE_C_COMPILER clang)
set(CMAKE_CXX_COMPILER clang++)

set(CMAKE_CXX_FLAGS "--std=c++11 -Weverything -Werror -Wno-c++98-compat -Wno-missing-prototypes -Wno-padded -fno-exceptions")

set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -DSGL_DEBUG")

add_executable(test ${sources})
