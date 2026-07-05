#pragma once

#include "mlir/Pass/Pass.h"
#include <memory>

namespace mnrt {
    std::unique_ptr<mlir::Pass> createConvertMemRefToMnrtPass();

    #define GEN_PASS_DECL
    #include "MnrtPasses.h.inc"
}