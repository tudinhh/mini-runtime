cmake_minimum_required(VERSION 3.15)
project(TinyTorchBackend)

set(CMAKE_CXX_STANDARD 17)

# Build the generic runtime
add_executable(tiny-runner runtime/runner.cpp)

target_include_directories(tiny-runner PRIVATE runtime/include)

target_link_libraries(tiny-runner ${CMAKE_DL_LIBS})
