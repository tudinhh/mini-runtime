add_mlir_library(MnrtConversionPasses
  ConvertMemRefToMnrt.cpp

  DEPENDS
  MnrtPassesIncGen

  LINK_LIBS PUBLIC
  MnrtDialect
  MLIRIR
  MLIRPass
  MLIRTransforms
  MLIRMemRefDialect
)