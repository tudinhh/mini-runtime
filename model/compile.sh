#!/bin/bash
set -e
. ../config.sh
# 1. Lower Linalg to LLVM IR (with C-wrapper interface for easy calling)
$MLIR_OPT ./build/model_linalg.mlir \
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
  -o ./build/model_llvm.mlir
# 2. Translate MLIR LLVM dialect to actual LLVM IR
$TRANSLATE -mlir-to-llvmir ./build/model_llvm.mlir > ./build/model.ll

$LLC -filetype=obj build/model.ll -o build/model.o

# 3. Compile LLVM IR to a shared library (.so)
$CLANG -shared build/model.o -o build/model.so

echo "Successfully compiled model.so"