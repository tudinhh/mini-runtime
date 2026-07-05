include "mlir/Pass/PassBase.td"

def ConvertMemRefToMnrt : Pass<"convert-memref-to-mnrt"> {
  let summary = "Lower standard memref allocations to mnrt hardware allocations.";
  let description = [{
    Finds `memref.alloc` operations and replaces them with `mnrt.alloc`,
    assigning a default memory space (e.g., DRAM = 0) if not specified.
  }];
  let dependentDialects = ["mnrt::MnrtDialect"];
}