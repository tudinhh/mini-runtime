## About the model
A simple Pytorch model that adds two tensors.

## Compilation
### Capture the model and translate to MLIR
The model is captured by Pytorch. Then, it is compiled into `torch` dialect.
```Python
module_torch = torchscript.compile(
    model, 
    (a, b), 
    output_type="torch"
)
```
At this point, the IR still shows the high-level semantics of the model with ATen operations.
```
    %0 = torch.aten.add.Tensor %arg0, %arg1, %int1 : ...

```
The second option is translated into `linalg` dialect with option `linalg-on-tensors`. After this, the IR in add_linalg.mlir starts losing the high-level architecture of the model.

### Further lowering
Once the IR reach `linalg` dialect, the compiler performs a series of transformations. Each trasformation pass is added manually in compile.sh such as `-convert-linalg-to-loops`.

Then, the file is compiled into an object file.

To summarize, the model undergoes a series of transformations: PyTorch model → torch dialect → linalg dialect  → LLVM IR → executable. It loses high-level semantics after each pass.

### Runtime
The run file does:
- Allocate the memory for input and output tensors
- Wrap the arrays into `MemRefDescriptor` to pass into MLIR code
- Call `_mlir_ciface_forward` to run the model.
So, basically, this run time allocates the memory, packs them it an appropriate way, then calls the model.
## Pop-up questions
**This experiment uses `torch-mlir` to compile a model. What is the default compilation of Pytorch?**

Pytorch uses TorchInductor as the primary compiler backend. TorchInductor lowers the model graph then generates Triton kernels.

**A simple runtime does: allocate -> organize -> execute. How about the production-grade runtimes like IREE and ONNX Runtime?**

More advanced memory management, quantization, handling heterogeneous architecture, and more.

**The idea of AI compiler**

Many passes are applied on the original model graph to transform and optimize at several levels of abstraction.


