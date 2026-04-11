#!/bin/bash
set -e

TORCH_MLIR_BUILD="/home/anhtu/torch-mlir/build"
TRANSLATE="$TORCH_MLIR_BUILD/bin/mlir-translate"
LLC="$TORCH_MLIR_BUILD/bin/llc"
CLANG="/usr/bin/clang++"
LIB_PATH="$TORCH_MLIR_BUILD/lib"
CUSTOM_OPT="../../custom-opt/build/bin/custom-opt"

MODEL_NAME="mobilenet" 

echo "Compiling ${MODEL_NAME}..."

# 1. Apply pipelines utilizing your direct OpenBLAS lowering
$CUSTOM_OPT build/${MODEL_NAME}.mlir \
  -pass-pipeline="builtin.module(linalg-to-bufferization,bufferization-to-llvm)" \
  -o build/${MODEL_NAME}_llvm.mlir

echo "- Translate to LLVM IR"
$TRANSLATE -mlir-to-llvmir build/${MODEL_NAME}_llvm.mlir -o build/${MODEL_NAME}.ll

echo "- Compile to object file"
$LLC -O3 -filetype=obj -relocation-model=pic build/${MODEL_NAME}.ll -o build/${MODEL_NAME}.o

echo "- Compile run.cpp"
$CLANG -O3 run.cpp build/${MODEL_NAME}.o -o build/run \
  -L$LIB_PATH \
  -L/usr/lib/x86_64-linux-gnu \
  -lmlir_c_runner_utils \
  -lopenblas \
  -Wl,-rpath,$LIB_PATH

echo "Compilation successful."