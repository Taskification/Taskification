#include "clang/Frontend/FrontendPluginRegistry.h"
#include "clang/AST/AST.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Sema/Sema.h"
#include "llvm/Support/raw_ostream.h"


using namespace clang;

namespace {


//-------------------ASTConsumer------------------------------//

class PrintCallExprConsumer : public ASTConsumer {
  CompilerInstance &Instance;
  std::set<std::string> ParsedTemplates;

public:

  PrintCallExprConsumer(CompilerInstance &Instance,
                         std::set<std::string> ParsedTemplates)
      : Instance(Instance), ParsedTemplates(ParsedTemplates) {}


  bool HandleTopLevelDecl(DeclGroupRef DG) override {
    for (DeclGroupRef::iterator i = DG.begin(), e = DG.end(); i != e; ++i) {
	const	 CallExpr *call=nullptr ;
	const    Decl* D =call->getCalleeDecl();
	if (*i==D) {
	
	
      if (const NamedDecl *ND = dyn_cast<NamedDecl>(call->getCalleeDecl ()))
        llvm::errs() << "fonction non pure: \"" << ND->getNameAsString() << "\"\n";
    }
	}
    return true;
  }



  void HandleTranslationUnit(ASTContext& context) override {
    if (!Instance.getLangOpts().DelayedTemplateParsing)
      return;


//-------------------RecursiveASTVisitor--------------------------

    struct Visitor : public RecursiveASTVisitor<Visitor> {
      const std::set<std::string> &ParsedTemplates;
      Visitor(const std::set<std::string> &ParsedTemplates)
          : ParsedTemplates(ParsedTemplates) {}

/*
      bool VisitFunctionDecl(FunctionDecl *FD) {
        if (FD->isLateTemplateParsed() &&
            ParsedTemplates.count(FD->getNameAsString()))
          LateParsedDecls.insert(FD);
        return true;

      }
*/
	bool VisitStmt(Stmt *st) {
	if (CallExpr *call = dyn_cast<CallExpr>(st)) {
	FunctionDecl *FD ;
	FD = call->getDirectCallee() ;
        if (FD ->isLateTemplateParsed() &&
        ParsedTemplates.count(FD->getNameAsString()))
        LateParsedDecls.insert(FD);

	}
        return true;
      }

      std::set<FunctionDecl*> LateParsedDecls;
    } v(ParsedTemplates);
    v.TraverseDecl(context.getTranslationUnitDecl());
    clang::Sema &sema = Instance.getSema();
    for (const FunctionDecl *FD : v.LateParsedDecls) {
      clang::LateParsedTemplate &LPT =
          *sema.LateParsedTemplateMap.find(FD)->second;
      sema.LateTemplateParser(sema.OpaqueParser, LPT);
      llvm::errs() << "late-parsed-decl: \"" << FD->getNameAsString() << "\"\n";
    }   
  }

};






//---------------------ASTAction--------------------------------------
class PrintCallExprAction : public PluginASTAction {
  std::set<std::string> ParsedTemplates;
protected:
  std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &CI,
                                                 llvm::StringRef) override {
    return std::make_unique<PrintCallExprConsumer>(CI, ParsedTemplates);
  }


  bool ParseArgs(const CompilerInstance &CI,
                 const std::vector<std::string> &args) override {
    for (unsigned i = 0, e = args.size(); i != e; ++i) {
      llvm::errs() << "PrintFunctionNames arg = " << args[i] << "\n";

      // Example error handling.
      DiagnosticsEngine &D = CI.getDiagnostics();
      if (args[i] == "-an-error") {
        unsigned DiagID = D.getCustomDiagID(DiagnosticsEngine::Error,
                                            "invalid argument '%0'");
        D.Report(DiagID) << args[i];
        return false;
      } else if (args[i] == "-parse-template") {
        if (i + 1 >= e) {
          D.Report(D.getCustomDiagID(DiagnosticsEngine::Error,
                                     "missing -parse-template argument"));
          return false;
        }
        ++i;
        ParsedTemplates.insert(args[i]);
      }
    }
    if (!args.empty() && args[0] == "help")
      PrintHelp(llvm::errs());

    return true;
  }
  void PrintHelp(llvm::raw_ostream& ros) {
    ros << "Help for PrintFunctionNames plugin goes here\n";
  }



};

}

//register the  Plugin so that Clang can call it during the build process
static FrontendPluginRegistry::Add<PrintCallExprAction>
	X("print-callexpr",//argument string that will be used to invoke the  Plugin
	 "print callexpr");//description of what the  Plugin does

