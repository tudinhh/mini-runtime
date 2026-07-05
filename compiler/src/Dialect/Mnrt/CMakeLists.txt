add_mlir_dialect_library(MnrtDialect
  MnrtDialect.cpp
  
  DEPENDS
  MnrtOpsIncGen

  LINK_LIBS PUBLIC
  MLIRIR
  MLIRSideEffectInterfaces
)