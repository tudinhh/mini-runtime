#include "Dialect/Mnrt/MnrtDialect.h"
#include "Dialect/Mnrt/MnrtOps.h"
#include "Dialect/Mnrt/MnrtPasses.h"
#include "mlir/Dialect/Func/IR/FuncOps.h"
#include "mlir/Transforms/DialectConversion.h"
#include "mlir/Transforms/GreedyPatternRewriteDriver.h"

using namespace mlir;

namespace {

struct DispatchToCallPattern : public OpRewritePattern<mnrt::DispatchOp> {
  using OpRewritePattern<mnrt::DispatchOp>::OpRewritePattern;

  LogicalResult matchAndRewrite(mnrt::DispatchOp op,
                                PatternRewriter& rewriter) const override {
    auto module = op->getParentOfType<ModuleOp>();
    StringRef symName = "mnrt_hal_submit";

    // 1. Insert the external C-ABI function declaration if missing
    if (!module.lookupSymbol(symName)) {
      OpBuilder::InsertionGuard guard(rewriter);
      rewriter.setInsertionPointToStart(module.getBody());
      auto funcType = rewriter.getFunctionType(op->getOperandTypes(), {});
      auto func = func::FuncOp::create(op.getLoc(), symName, funcType);
      func.setPrivate();
      rewriter.insert(func);
    }

    // 2. Replace the hardware dispatch with a standard C function call
    rewriter.replaceOpWithNewOp<func::CallOp>(op, symName, TypeRange{},
                                              op.getOperands());
    return success();
  }
};

struct ConvertMnrtToLLVMPass
    : public PassWrapper<ConvertMnrtToLLVMPass, OperationPass<ModuleOp>> {
  MLIR_DEFINE_EXPLICIT_INTERNAL_INLINE_TYPE_ID(ConvertMnrtToLLVMPass)

  StringRef getArgument() const final { return "convert-mnrt-to-llvm"; }
  StringRef getDescription() const final {
    return "Lower mnrt dispatch to external HAL calls.";
  }

  void runOnOperation() override {
    RewritePatternSet patterns(&getContext());
    patterns.add<DispatchToCallPattern>(&getContext());
    if (failed(applyPatternsGreedily(getOperation(), std::move(patterns)))) {
      signalPassFailure();
    }
  }
};

}  // end anonymous namespace

namespace mnrt {
std::unique_ptr<mlir::Pass> createConvertMnrtToLLVMPass() {
  return std::make_unique<ConvertMnrtToLLVMPass>();
}
}  // namespace mnrt