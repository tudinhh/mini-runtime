cmake_minimum_required(VERSION 3.15)
project(TinyTorchBackend)

set(CMAKE_CXX_STANDARD 17)

# Build the generic runtime
add_executable(tiny-runner runtime/runner.cpp)

# Link against the OS dynamic loader library (dl)
# Linux requires dl, macOS usually includes it in libc automatically

target_link_libraries(tiny-runner ${CMAKE_DL_LIBS})
