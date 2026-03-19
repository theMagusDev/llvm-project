#include "clang/AST/AST.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/Attr.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendPluginRegistry.h"
#include "clang/Basic/Diagnostic.h"

using namespace clang;

namespace {

class AnnotateUnusedVarsVisitor : public RecursiveASTVisitor<AnnotateUnusedVarsVisitor> {
public:
  explicit AnnotateUnusedVarsVisitor(ASTContext &Context, DiagnosticsEngine &Diags)
      : Context(Context), Diags(Diags) {
    DiagID = Diags.getCustomDiagID(
      DiagnosticsEngine::Warning,
      "variable '%0' is unused; annotating with [[maybe_unused]]"
    );
  }

bool VisitVarDecl(VarDecl *VD) {
    if (!VD || VD->isImplicit())
      return true;

    bool IsLocal = VD->isLocalVarDecl();
    bool IsParm = isa<ParmVarDecl>(VD);
    if (!IsLocal && !IsParm)
      return true;

    const DeclContext *DC = VD->getDeclContext();
    if (!DC || !DC->isFunctionOrMethod())
      return true;

    SourceLocation Loc = VD->getLocation();
    SourceManager &SM = Context.getSourceManager();
    if (Loc.isInvalid() || SM.isInSystemHeader(Loc) || SM.isInSystemMacro(Loc))
      return true;

    if (VD->hasAttr<UnusedAttr>() || VD->isUsed() || VD->isReferenced())
      return true;

    auto *Attr = UnusedAttr::CreateImplicit(Context, Loc, UnusedAttr::CXX11_maybe_unused);
    VD->addAttr(Attr);

    Diags.Report(Loc, DiagID) << VD->getNameAsString();

    return true;
  }

private:
  ASTContext &Context;
  DiagnosticsEngine &Diags;
  unsigned DiagID;
};

class AnnotateUnusedVarsConsumer : public ASTConsumer {
public:
  explicit AnnotateUnusedVarsConsumer(ASTContext &Context, DiagnosticsEngine &Diags)
      : Visitor(Context, Diags) {}

  void HandleTranslationUnit(ASTContext &Context) override {
    Visitor.TraverseDecl(Context.getTranslationUnitDecl());
  }

private:
  AnnotateUnusedVarsVisitor Visitor;
};

class AnnotateUnusedVarsAction : public PluginASTAction {
protected:
  std::unique_ptr<ASTConsumer> CreateASTConsumer(
    CompilerInstance &CI,
    llvm::StringRef
  ) override {
    return std::make_unique<AnnotateUnusedVarsConsumer>(
      CI.getASTContext(),
      CI.getDiagnostics()
    );
  }

  bool ParseArgs(
    const CompilerInstance &CI,
    const std::vector<std::string> &Args
  ) override {
    return true;
  }

  ActionType getActionType() override {
    return AddBeforeMainAction;
  }
};

} // namespace

static FrontendPluginRegistry::Add<AnnotateUnusedVarsAction>
    X("annotate-unused-vars", "Annotates unused locals with [[maybe_unused]]");