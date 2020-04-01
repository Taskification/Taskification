#include "clang/Frontend/FrontendPluginRegistry.h"
#include "clang/AST/AST.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"

#include <iostream>

using namespace clang;


class ImpureExprVisitor
	: public RecursiveASTVisitor <ImpureExprVisitor> {
public:
	/***********************
	* MY PLUGIN FUNCTIONS *
	***********************/

	/* Small Helper to print location */
	void print_decl_location(Decl *D)
	{
		ASTContext&   Context    = D->getASTContext();
		FullSourceLoc source_loc = Context.getFullLoc(D->getLocation() );

		source_loc.dump();
	}

	/*****************************
	* FUNCTIONS CALLED BY CLANG *
	*****************************/
	/* This is called on every decls */
	bool VisitDecl(Decl *D)
	{
		/* It it a Function decl ?? */
		FunctionDecl* fD = dyn_cast <FunctionDecl>(D);

		if(fD)
		{
			//fD->dump();
			this->function_decl_list.push_back(fD);
		}

		return true;
	}

	/* This is called once for every variable decl */
	bool VisitVarDecl(VarDecl *vD)
	{
		/* Called for each global variable */
		if(vD->hasGlobalStorage() )
		{
			//vD->dump();
			this->global_var_list.push_back(vD);
		}
		return true;
	}

	std::vector <VarDecl *> & global_vars_get()
	{
		return this->global_var_list;
	}

	std::vector <FunctionDecl *> & functions_get()
	{
		return this->function_decl_list;
	}

private:
	std::vector <VarDecl *> global_var_list;
	std::vector <FunctionDecl *> function_decl_list;
};


//-------------------ASTConsumer------------------------------//


class ImpureExprConsumer : public clang::ASTConsumer {
public:


	/*************************
	 * PLUGIN IMPLEMENTATION *
	 *************************/
	void check_impure_function(FunctionDecl *fn, std::vector <VarDecl *> &vars)
	{
		if(fn->hasBody())
		{
			std::cout << "==== Exploring " << fn->getNameInfo().getAsString() << std::endl;

			Stmt * body = fn->getBody();

			for (Stmt::child_iterator i = body->child_begin(), e = body->child_end(); i != e; ++i) {
				Stmt *currStmt = *i;
				currStmt->dump();

				/* Check for function calls */

				/* TODO */

				/* Check for References to global variables ... */


  			}
		}	
	}

	/* This functions is called by llvm */

	virtual void HandleTranslationUnit(clang::ASTContext&Context)
	{
		/* We now visit all the declarations in the TU */
		Visitor.TraverseDecl(Context.getTranslationUnitDecl() );

		std::cout << "================" << std::endl;
		std::cout << "Functions" << std::endl;
		std::cout << "================" << std::endl;

		/* Our visitor now has all the elements to be scanned */
		std::vector <FunctionDecl *> &funcs = this->Visitor.functions_get();
		std::vector <VarDecl *> &vars = this->Visitor.global_vars_get();

		/* Walk each function */
		for(unsigned int i = 0 ; i < funcs.size(); i++)
		{
			//funcs[i]->dump();

			check_impure_function(funcs[i], vars);
		}

		std::cout << "================" << std::endl;
		std::cout << "Global Variables" << std::endl;
		std::cout << "================" << std::endl;

		for(unsigned int  i = 0 ; i < vars.size(); i++)
		{
			vars[i]->dump();
		}
	}

private:

	// A RecursiveASTVisitor implementation.
	ImpureExprVisitor Visitor;
};

//---------------------ASTAction--------------------------------------
class ImpureAction : public PluginASTAction {
	std::set <std::string> ParsedTemplates;
protected:
	std::unique_ptr <ASTConsumer> CreateASTConsumer(CompilerInstance&CI,
	                                                llvm::StringRef) override
	{
		return std::make_unique <ImpureExprConsumer>();
	}

	bool ParseArgs(const CompilerInstance&CI,
	               const std::vector <std::string>&args) override
	{
		for(unsigned i = 0, e = args.size(); i != e; ++i)
		{
			llvm::errs() << "arg = " << args[i] << "\n";
		}

		return true;
	}
};


//register the  Plugin so that Clang can call it during the build process
static FrontendPluginRegistry::Add <ImpureAction>
X("impurewarnings",        //argument string that will be used to invoke the  Plugin
  "print callexpr");       //description of what the  Plugin does
