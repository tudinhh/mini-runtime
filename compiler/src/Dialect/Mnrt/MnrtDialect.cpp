#include "Dialect/Mnrt/MnrtDialect.h"
#include "Dialect/Mnrt/MnrtOps.h"

using namespace mlir;
using namespace mnrt;

// Remove the path prefix here
#include "MnrtOpsDialect.cpp.inc"

void MnrtDialect::initialize() {
  addOperations<
#define GET_OP_LIST

#include "MnrtOps.cpp.inc"
      >();
}

#define GET_OP_CLASSES

#include "MnrtOps.cpp.inc"