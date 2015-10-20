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
	};

	struct bcScript
	{
		bool isCompiled;
		std::vector<std::string> src;
		bc::vm::bcExecContext* con;
	}; 
	
	struct bcContext
	{
		int loadScript(bcScript);
		bcScript scr;
		vm::bcVM vm;
		bcContextData data;
	};


	int startup(bcContext*);
	int shutdown(bcContext*);

	//load script
	bcScript loadScriptFromFile(bcContext*, std::string);
	bcScript loadScriptFromString(bcContext*, std::string);

	//
	int runScriptFromFile();
}
#endif