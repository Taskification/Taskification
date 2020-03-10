#include "clang/Driver/Options.h"
#include "clang/AST/AST.h"
#include "clang/AST/Decl.h"
#include "clang/AST/ASTContext.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/ASTConsumers.h"
#include "clang/Frontend/FrontendActions.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendPluginRegistry.h"
#include "clang/Rewrite/Core/Rewriter.h"

using namespace std;
using namespace clang;
using namespace llvm;

// L'idée est de détecter les fonctions non-pures et les re-écrire dans un fichiers à part 

Rewriter rewriter; // une instance de la classe rewriter qui sera utilisée pour modifier le code source d'origine. 
                   // (en rajoutant un msg vide à la detection de la fonction impure)
                  // (globale : sera utiliser par plusieurs classes)


class TaskVisitor {
private:
    ASTContext *astContext; // used for getting additional AST info
 
public:
    explicit TaskVisitor(CompilerInstance *CI)
        : astContext(&(CI->getASTContext())) // initialize private members
    {
        rewriter.setSourceMgr(astContext->getSourceManager(),
            astContext->getLangOpts());
    }

    // on doit retourner la liste des variables et analyser les si ils rendrent la fonction non-pures
    // fonction qui retourne vrai si la fonction est pure
      
    virtual bool VisitFunctionParam() {
        // code
        return true;
    }     


    virtual bool principale(Stmt *st) {
       // code
        return true;
    }

};



class TaskASTConsumer : public ASTConsumer {
private:
    TaskVisitor *visitor; // !!

    
    // Function to get the base name of the file provided by path
    string basename(std::string path) {
        return std::string( std::find_if(path.rbegin(), path.rend(), MatchPathSeparator()).base(), path.end());
    }

    // Used by std::find_if
    struct MatchPathSeparator
    {
        bool operator()(char ch) const {
            return ch == '/';
        }
    };
 
public:
    explicit TaskASTConsumer(CompilerInstance *CI)
        : visitor(new TaskVisitor(CI)) // initialize the visitor
        { }
 
    // idée à revoire

    // Créer un fichier de sortie pour écrire les fonctions impures (aprés qu'on les detectes on peut les modifier en rajoutant 
    // un affichage d'un message vide pour faciliter leurs re-ecritures)

    virtual void HandleTranslationUnit(ASTContext &Context) {
        //visitor->TraverseDecl(Context.getTranslationUnitDecl());

        FileID id = rewriter.getSourceMgr().getMainFileID();
        string filename = "/tmp/" + basename(rewriter.getSourceMgr().getFilename(rewriter.getSourceMgr().getLocForStartOfFile(id)).str());
        std::error_code OutErrorInfo;
        std::error_code ok;
        llvm::raw_fd_ostream outFile(llvm::StringRef(filename),
            OutErrorInfo, llvm::sys::fs::F_None);
        if (OutErrorInfo == ok) {
            const RewriteBuffer *RewriteBuf = rewriter.getRewriteBufferFor(id);
            outFile << std::string(RewriteBuf->begin(), RewriteBuf->end());
            errs() << "Output file created - " << filename << "\n";
        } else {
            llvm::errs() << "Could not create file\n";
        }
    }

};


// notre propre PluginASTAction personnalisé,
// qui est simplement une classe de base abstraite à utiliser pour les plugins AST grand public
class PluginTaskAction : public PluginASTAction {
protected:
    // CreateASTConsumer : qui est appelé par clang quand il appelle notre plugin.
    // C'est l'endroit d'où nous appelons et retournons notre consommateur AST personnalisé (TaskASTConsumer).
    unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &CI, StringRef file) {
        return make_unique<TaskASTConsumer>(&CI);
    }


    // ParseArgs : qui est nécessaire pour analyser les arguments de ligne de commande personnalisés s'ils existes
    bool ParseArgs(const CompilerInstance &CI, const vector<string> &args) {
        return true;
    }
};

 //Les deux sections précédentes visaient simplement à mettre en place une infrastructure.


 // pour enregistrer le plugin pour que Clang puisse l'appeler pendant le processus de construction. 
static FrontendPluginRegistry::Add<PluginTaskAction> X("-task-plugin", "Plugin de Taskification");
