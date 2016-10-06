#ifndef WEEC_WEEC_H
#define WEEC_WEEC_H
/*
Wee C API
0.0.0.0-1

Lightweight embedabble scripting language
*/
#include "util.h"
#include "error.h"
#include "lexer.h"
#include "parser.h"
#include "bytecode.h"
#include "compiler.h"
#include "vm.h"
#include "import.h"
#include "export.h"

//API
namespace wc
{
	struct wcContextData
	{
		unsigned int id;
		bool alwaysAttachScriptOnRun;
	};

	struct wcScript
	{
		wcScript();
		wcScript(std::string input, bool isFile);
		void clear();

		int load(std::string);		//load script source code into this object
		int load(std::vector<std::string>*);
		int loadFromFile(std::string);

		bool isCompiled, isExecuted;
		std::vector<std::string> src;
		vm::wcExecContext* con;
	};

	struct wcContext
	{
		wcScript scr;
		vm::wcVM vm;
		wcContextData data;
	};

	int startup(wcContext*);
	int shutdown(wcContext*);

	//compile script
	int compileScript(wcScript*);

	//run
	int run(wcContext*, wcScript*);
	int run(wcContext*, vm::wcExecContext*);

	//from file
	int runScriptFromFile(wcContext*, std::string);

	//symbol query
	bool doesSymbolExist(wcScript*, std::string p_sym);
	vm::wcVMSymbol getSymbol(wcScript*, std::string p_sym);
	int getSymbolValue(wcScript*, std::string p_sym);
	bool doesStoredFrameExist(wcScript*, int p_sid);
}

#endif //WEEC_WEEC_H