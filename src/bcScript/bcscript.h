#ifndef BC_BCSCRIPT_H
#define BC_BCSCRIPT_H
/*
bcScript API
0.0.0.0-1

Lightweight embedabble scripting language
*/
#include "config.h"
#include "util.h"
#include "error.h"
#include "lexer.h"
#include "parser.h"
#include "bytecode.h"
#include "vm.h"
#include "compiler.h"
#include "import.h"
#include "export.h"

//API
namespace bc
{
	struct bcContextData
	{
		unsigned int id;
		bool alwaysAttachScriptOnRun;
	};

	struct bcScript
	{
		void clear();

		int load(std::string);
		int load(std::vector<std::string>*);
		int loadFromFile(std::string);

		bool isCompiled,isExecuted;
		std::vector<std::string> src;
		bc::vm::bcExecContext* con;
	}; 
	
	struct bcContext
	{
		bcScript scr;
		vm::bcVM vm;
		bcContextData data;
	};

	int startup(bcContext*);
	int shutdown(bcContext*);

	//compile script
	int compileScript(bcScript*);
	
	//run
	int run(bcContext*, bcScript*);
	int run(bcContext*, vm::bcExecContext*);
	int runScriptFromFile(bcContext*, std::string);

	//symbol query
	bool doesSymbolExist(bcScript*, std::string p_sym);
	vm::bcVMSymbol getSymbol(bcScript*, std::string p_sym);
	int getSymbolValue(bcScript*, std::string p_sym);
	bool doesStoredFrameExist(bcScript*, int p_sid);
}
#endif