#ifndef LLVM_TRANSFORMS_UTILS_TEST2_H
#define LLVM_TRANSFORMS_UTILS_TEST2_H

#include "llvm/IR/PassManager.h"

namespace llvm {

class SimpleInlinerPass : public PassInfoMixin<SimpleInlinerPass> {
public:
  PreservedAnalyses run(Function& F, FunctionAnalysisManager& AM);
};

} // namespace llvm

#endif // LLVM_TRANSFORMS_UTILS_TEST2_H
