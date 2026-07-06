#include "mlir/Tools/mlir-opt/MlirOptMain.h"
#include "mlir/Dialect/Arith/IR/Arith.h"
#include "mlir/Dialect/Func/IR/FuncOps.h"
#include "mlir/Dialect/MemRef/IR/MemRef.h"
#include "mlir/Dialect/SCF/IR/SCF.h"
#include "mlir/Dialect/LLVMIR/LLVMDialect.h"
#include "Dialect/Mnrt/MnrtPasses.h"
#include "Dialect/Mnrt/MnrtDialect.h"
#include "mlir/Dialect/Linalg/IR/Linalg.h"
#include "mlir/Transforms/Passes.h"
#include "mlir/Conversion/AffineToStandard/AffineToStandard.h"
#include "mlir/Conversion/SCFToControlFlow/SCFToControlFlow.h"
#include "mlir/Conversion/MathToLLVM/MathToLLVM.h"
#include "mlir/Conversion/ArithToLLVM/ArithToLLVM.h"
#include "mlir/Conversion/MemRefToLLVM/MemRefToLLVM.h"
#include "mlir/Conversion/FuncToLLVM/ConvertFuncToLLVMPass.h"
#include "mlir/Conversion/ControlFlowToLLVM/ControlFlowToLLVM.h"
#include "mlir/Conversion/ReconcileUnrealizedCasts/ReconcileUnrealizedCasts.h"
#include "mlir/Dialect/MemRef/Transforms/Passes.h"
#include "mlir/Dialect/LLVMIR/Transforms/Passes.h"
#include "mlir/Conversion/Passes.h"
#include "mlir/Dialect/Affine/Transforms/Passes.h"
#include "mlir/Dialect/Bufferization/Transforms/Passes.h"
#include "Dialect/Mnrt/MnrtPasses.h"
#include "mlir/Conversion/AffineToStandard/AffineToStandard.h"
#include "mlir/Dialect/Linalg/Passes.h"
#include "mlir/Dialect/Tensor/IR/Tensor.h"
#include "mlir/Dialect/Linalg/Transforms/BufferizableOpInterfaceImpl.h"
#include "mlir/Dialect/Tensor/Transforms/BufferizableOpInterfaceImpl.h"
#include "mlir/Dialect/Bufferization/Transforms/FuncBufferizableOpInterfaceImpl.h"

namespace mnrt {
  void registerMnrtPasses();
}

int main(int argc, char **argv) {
  mlir::DialectRegistry registry;
  
  registry.insert<
      mlir::arith::ArithDialect,
      mlir::func::FuncDialect,
      mlir::memref::MemRefDialect,
      mlir::scf::SCFDialect,
      mlir::LLVM::LLVMDialect,
      mlir::linalg::LinalgDialect,
      mlir::tensor::TensorDialect,
      mnrt::MnrtDialect
  >();
  mnrt::registerMnrtPasses();
  mlir::affine::registerAffinePasses();
  mlir::registerSCFToControlFlowPass();
  mlir::registerConvertMathToLLVMPass();
  mlir::registerArithToLLVMConversionPass();
  mlir::registerFinalizeMemRefToLLVMConversionPass();
  mlir::registerConvertFuncToLLVMPass();
  mlir::registerConvertControlFlowToLLVMPass();
  mlir::registerReconcileUnrealizedCastsPass();
  mlir::memref::registerMemRefPasses();
  mlir::LLVM::registerLLVMPasses();
  mlir::bufferization::registerBufferizationPasses();
  mlir::registerLinalgPasses();
// Register the MLIR Affine lowering pass
  mlir::registerLowerAffinePass();
  mlir::linalg::registerBufferizableOpInterfaceExternalModels(registry);
mlir::tensor::registerBufferizableOpInterfaceExternalModels(registry);
mlir::bufferization::func_ext::registerBufferizableOpInterfaceExternalModels(registry);
  return mlir::asMainReturnCode(
      mlir::MlirOptMain(argc, argv, "Mnrt Optimizer Driver\n", registry));
}