include "mlir/Pass/PassBase.td"

def ConvertMemRefToMnrt : Pass<"convert-memref-to-mnrt"> {
  let summary = "Lower MemRef calls to Mnrt dispatch operations.";
  let constructor = "mnrt::createConvertMemRefToMnrtPass()";
  let dependentDialects = ["mnrt::MnrtDialect"];
}

def ConvertMnrtToLLVM : Pass<"convert-mnrt-to-llvm", "ModuleOp"> {
  let summary = "Lower mnrt operations to LLVM dialect.";
  let constructor = "mnrt::createConvertMnrtToLLVMPass()";
  let dependentDialects = ["func::FuncDialect"];
}