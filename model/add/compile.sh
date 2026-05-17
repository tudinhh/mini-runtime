#!/bin/bash
set -e 

TORCH_MLIR_BUILD="/home/anhtu/torch-mlir/build"
MLIR_OPT="$TORCH_MLIR_BUILD/bin/mlir-opt"
TRANSLATE="$TORCH_MLIR_BUILD/bin/mlir-translate"
LLC="$TORCH_MLIR_BUILD/bin/llc"
CLANG="/usr/bin/clang++"
BUILD="./build"

echo "1. Lowering MLIR to LLVM Dialect..."
$MLIR_OPT $BUILD/add_linalg.mlir \
  -empty-tensor-to-alloc-tensor \
  -one-shot-bufferize="bufferize-function-boundaries=1" \
  -convert-linalg-to-loops \
  -lower-affine \
  -convert-scf-to-cf \
  -expand-strided-metadata \
  -llvm-request-c-wrappers \
  -convert-math-to-llvm \
  -convert-arith-to-llvm \
  -finalize-memref-to-llvm \
  -convert-func-to-llvm \
  -convert-cf-to-llvm \
  -reconcile-unrealized-casts \
  -o $BUILD/add_llvm.mlir

echo "2. Translating to LLVM IR..."
$TRANSLATE -mlir-to-llvmir $BUILD/add_llvm.mlir > $BUILD/add.ll

echo "3. Compiling Object File..."
$LLC -filetype=obj $BUILD/add.ll -o $BUILD/add.o

echo "4. Compileing the run file..."
$CLANG -O3 run.cpp $BUILD/add.o -o $BUILD/run \
  -L/home/anhtu/torch-mlir/build/lib \
  -lmlir_c_runner_utils \
  -Wl,-rpath,/home/anhtu/torch-mlir/build/lib