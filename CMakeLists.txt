cmake_minimum_required(VERSION 3.15)
project(TinyTorchBackend)

set(CMAKE_CXX_STANDARD 17)

add_executable(mnrt-runner runtime/runner.cpp)
target_include_directories(mnrt-runner PRIVATE runtime/include)
target_link_libraries(mnrt-runner ${CMAKE_DL_LIBS})
