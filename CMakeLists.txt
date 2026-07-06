cmake_minimum_required(VERSION 3.15)
project(TinyTorchBackend)

set(MLIR_DIR "/home/anhtu/torch-mlir/externals/llvm-project/build/lib/cmake/mlir" CACHE PATH "Path to MLIR config")
set(LLVM_DIR "/home/anhtu/torch-mlir/externals/llvm-project/build/lib/cmake/llvm" CACHE PATH "Path to LLVM config")

set(CMAKE_CXX_STANDARD 17)

# Move the packages and macros here, before add_subdirectory
find_package(MLIR REQUIRED CONFIG)
find_package(LLVM REQUIRED CONFIG)

list(APPEND CMAKE_MODULE_PATH "${MLIR_CMAKE_DIR}")
list(APPEND CMAKE_MODULE_PATH "${LLVM_CMAKE_DIR}")
include(TableGen)
include(AddLLVM)
include(AddMLIR)

add_subdirectory(compiler)
add_subdirectory(runtime)