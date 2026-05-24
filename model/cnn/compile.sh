#!/bin/bash
set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" &> /dev/null && pwd)"
source "$SCRIPT_DIR/../../config.sh"

LIB_PATH="$TORCH_MLIR_BUILD/lib"

sed -i 's/func.func @main\(.*\) {/func.func @forward\1 attributes {llvm.emit_c_interface} {/g' build/cnn.mlir

echo "1. Apply MLIR transformations"
$MLIR_OPT build/cnn.mlir \
  -empty-tensor-to-alloc-tensor \
  -one-shot-bufferize="bufferize-function-boundaries=1" \
  -convert-linalg-to-loops \
  -lower-affine \
  -convert-scf-to-cf \
  -convert-cf-to-llvm \
  -convert-math-to-llvm \
  -convert-math-to-libm \
  -convert-arith-to-llvm \
  -convert-vector-to-llvm \
  -expand-strided-metadata \
  -finalize-memref-to-llvm \
  -convert-func-to-llvm="use-bare-ptr-memref-call-conv=0" \
  -reconcile-unrealized-casts \
  -o build/cnn_llvm.mlir

echo "2. Translate MLIR to LLVM IR"
$TRANSLATE -mlir-to-llvmir  build/cnn_llvm.mlir -o  build/cnn.ll

echo "3. Compile LLVM IR to .o"
$LLC -O3 -filetype=obj -relocation-model=pic build/cnn.ll -o  build/cnn.o

echo "4. Compile run.cpp"
$CLANG -O3 run.cpp  build/cnn.o -o  build/run \
  -L$TORCH_MLIR_DIR/build/lib \
  -lmlir_c_runner_utils \
  -Wl,-rpath,$TORCH_MLIR_DIR/build/lib

echo "Done!"