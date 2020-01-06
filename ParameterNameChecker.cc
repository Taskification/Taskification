#include "clang/AST/AST.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendPluginRegistry.h"
#include "llvm/Support/raw_ostream.h"

using namespace clang;

namespace {

// RecursiveASTVisitor does a pre-order depth-first traversal of the
// AST. We implement VisitFoo() methods for the types of nodes we are
// interested in.
class FuncDeclVisitor : public RecursiveASTVisitor<FuncDeclVisitor> {
public:
  explicit FuncDeclVisitor(DiagnosticsEngine &d) : m_diag(d) {}

  // This function gets called for each FunctionDecl node in the AST.
  // Returning true indicates that the traversal should continue.
  bool VisitFunctionDecl(const FunctionDecl *funcDecl) {
    if (const FunctionDecl *prevDecl = funcDecl->getPreviousDecl()) {
      // If one of the declarations is without prototype, we can't compare them.
      if (!funcDecl->hasPrototype() || !prevDecl->hasPrototype())
        return true;

      assert(funcDecl->getNumParams() == prevDecl->getNumParams());

      for (unsigned i = 0, e = funcDecl->getNumParams(); i != e; ++i) {
        const ParmVarDecl *paramDecl = funcDecl->getParamDecl(i);
        const ParmVarDecl *previousParamDecl = prevDecl->getParamDecl(i);

        // Ignore the case of unnamed parameters.
        if (paramDecl->getName() == "" || previousParamDecl->getName() == "")
          return true;

        if (paramDecl->getIdentifier() != previousParamDecl->getIdentifier()) {
          unsigned warn = m_diag.getCustomDiagID(DiagnosticsEngine::Warning,
              "parameter name mismatch");
          m_diag.Report(paramDecl->getLocation(), warn);

          unsigned note = m_diag.getCustomDiagID(DiagnosticsEngine::Note,
              "parameter in previous function declaration was here");
          m_diag.Report(previousParamDecl->getLocation(), note);
        }
      }
    }

    return true;
  }

private:
  DiagnosticsEngine &m_diag;
};

// An ASTConsumer is a client object that receives callbacks as the AST is
// built, and "consumes" it.
class FuncDeclConsumer : public ASTConsumer {
public:
  explicit FuncDeclConsumer(DiagnosticsEngine &d)
      : m_visitor(FuncDeclVisitor(d)) {}

  // Called by the parser for each top-level declaration group.
  // Returns true to continue parsing, or false to abort parsing.
  virtual bool HandleTopLevelDecl(DeclGroupRef dg) override {
    for (Decl *decl : dg) {
      m_visitor.TraverseDecl(decl);
    }

    return true;
  }

private:
  FuncDeclVisitor m_visitor;
};

class ParameterNameChecker : public PluginASTAction {
protected:
  // Create the ASTConsumer that will be used by this action.
  // The StringRef parameter is the current input filename (which we ignore).
  std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &ci,
                                                 llvm::StringRef) override {
    return llvm::make_unique<FuncDeclConsumer>(ci.getDiagnostics());
  }

  // Parse command-line arguments. Return true if parsing succeeded, and
  // the plugin should proceed; return false otherwise.
  bool ParseArgs(const CompilerInstance&,
                 const std::vector<std::string>&) override {
    // We don't care about command-line arguments.
    return true;
  }
};

} // end namespace

// Register the PluginASTAction in the registry.
// This makes it available to be run with the '-plugin' command-line option.
static FrontendPluginRegistry::Add<ParameterNameChecker>
X("check-parameter-names", "check for parameter names mismatch");
