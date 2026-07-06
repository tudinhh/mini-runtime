add_executable(mnrt-opt mnrt-opt.cpp)

add_dependencies(mnrt-opt MnrtOpsIncGen MnrtPassesIncGen)

target_link_libraries(mnrt-opt PRIVATE
  MnrtDialect
  MnrtConversionPasses
  MLIROptLib
  MLIRMemRefDialect
  MLIRFuncDialect
  MLIRArithDialect
  MLIRSCFDialect
  MLIRLLVMDialect
  MLIRLinalgDialect
  MLIRAffineToStandard
  MLIRSCFToControlFlow
  MLIRMathToLLVM
  MLIRArithToLLVM
  MLIRMemRefToLLVM
  MLIRFuncToLLVM
  MLIRControlFlowToLLVM
  MLIRReconcileUnrealizedCasts
  MLIRLLVMIRTransforms
  MLIRAffineTransforms
  MLIRAffineDialect
  MLIRMemRefTransforms
  MLIRBufferizationTransforms
  MLIRLinalgTransforms
  MLIRTensorDialect
  MLIRTensorTransforms
)