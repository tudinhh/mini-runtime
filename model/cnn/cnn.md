## About the Model
This targets a standard Convolutional Neural Network.

## Custom pass: OpenBLAS
The compilation process leverages a custom MLIR optimization pass to replace standard matrix computations with high-performance OpenBLAS calls.

### Create a custom mlir-opt (custom-opt.cpp)

Instead of adding transformation flags manually in build scipt (compile.sh), the passes are organized as pipelines and registered with the pass manager.

```
manager.addPass(mlir::createLowerAffinePass());
```
The pipelines are then registered also. With this convention, it's more easy to organize and create custom transformation order.

This experiment adds a custom pass to replace `linalg.matmul` with OpenBLAS. The implementation of the custom pass is written in ConvertMatmulToBlas.cpp.

### The custom pass (ConvertMatmulToBlas.cpp)
This file defines the conversion logic to replace `linalg.matmul` operations with OpenBLAS calls, utilizing the standard "Match and Rewrite" paradigm:

**Match**: Intercepts the target operation and verifies it meets the necessary constraints (e.g., correct tensor shapes and data types).

**Rewrite (Extract & Prepare)**: Extracts data pointers, generates necessary arguments, and creates lower-level operations (like an llvm.call) using the `Rewriter`.

**Replace/Erase**: Deletes the original operation from the compiler graph.

## Runtime
This time, a profiling code is added into `run.cpp` to compare the run time between the code with and without OpenBLAS:
- w/ OpenBLAS: 124 ms
- w/o OpenBLAS: 144 ms

## Pop-up Questions
**Generalize the pattern**

To intercept an operation A with operation B, follow these steps:
- Check if operands, types, attributes of A meet the preconditions to use B instead
- Extract arguments from A and process them to match arguments of B
- Invoke B and erase A
- Return

These steps are for the conversion itself, we still need to make it integrate into MLIR.

**The code use `ConversionPattern`. What are others?**
- `ConversionPattern` is used for dialect conversion, typically changes the types
- `RewritePattern` is used to transform the code in the same abstraction level like DCE and Constant Folding.