#!/bin/bash
set -e
. ../config.sh

# 1. Monolithic MLIR Pipeline (Device Abstraction + Host Lowering)
../build/compiler/tools/mnrt-opt ./build/input.mlir \
  --empty-tensor-to-alloc-tensor \
  --one-shot-bufferize="bufferize-function-boundaries=1" \
  --convert-linalg-to-loops \
  --convert-memref-to-mnrt \
  --convert-mnrt-to-llvm \
  --lower-affine \
  --convert-scf-to-cf \
  --expand-strided-metadata \
  --llvm-request-c-wrappers \
  --convert-math-to-llvm \
  --convert-arith-to-llvm \
  --finalize-memref-to-llvm \
  --convert-func-to-llvm \
  --convert-cf-to-llvm \
  --reconcile-unrealized-casts \
  -o ./build/model_llvm.mlir
# 2. Translation and Compilation
$TRANSLATE -mlir-to-llvmir ./build/model_llvm.mlir > ./build/model.ll
$LLC -filetype=obj build/model.ll -o build/model.o
$CLANG -shared build/model.o -o build/model.so

echo "Successfully compiled model.so"