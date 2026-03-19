#include "llvm/Transforms/Utils/SimpleInliner.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/Transforms/Utils/Cloning.h"
#include "llvm/Transforms/Utils/ValueMapper.h"
#include <vector>

using namespace llvm;

PreservedAnalyses SimpleInlinerPass::run(Function& F, FunctionAnalysisManager& AM) {
  bool Changed = false;
  std::vector<CallInst*> CallsToInline;

  for (BasicBlock& BB : F) {
    for (Instruction& I : BB) {
      if (auto* CI = dyn_cast<CallInst>(&I)) {
        Function* Callee = CI->getCalledFunction();
        if (Callee && !Callee->isDeclaration() &&
            Callee->getReturnType()->isVoidTy() && 
            Callee->arg_size() == 0) {
          CallsToInline.push_back(CI);
        }
      }
    }
  }

  for (CallInst* CI : CallsToInline) {
    Function* Callee = CI->getCalledFunction();
    BasicBlock* CallBlock = CI->getParent();
    
    BasicBlock* PostCallBlock = CallBlock->splitBasicBlock(CI->getIterator(), "post.call");

    ValueToValueMapTy VMap;
    std::vector<BasicBlock*> ClonedBlocks;

    for (BasicBlock& BB : *Callee) {
      BasicBlock* CBB = CloneBasicBlock(&BB, VMap, "cloned", &F);
      VMap[&BB] = CBB;
      ClonedBlocks.push_back(CBB);
    }

    for (BasicBlock* CBB : ClonedBlocks) {
      for (Instruction& I : *CBB) {
        RemapInstruction(&I, VMap, RF_NoModuleLevelChanges | RF_IgnoreMissingLocals);
      }
      
      if (ReturnInst* RI = dyn_cast<ReturnInst>(CBB->getTerminator())) {
        BranchInst::Create(PostCallBlock, CBB);
        RI->eraseFromParent();
      }
    }

    CallBlock->getTerminator()->eraseFromParent();
    BranchInst::Create(ClonedBlocks[0], CallBlock);

    CI->eraseFromParent();
    Changed = true;
  }

  return Changed ? PreservedAnalyses::none() : PreservedAnalyses::all();
}
