#ifndef WCC_H
#define WCC_H

#include "weec.h"
#include <string>

namespace wcc
{
	enum class CompilerAction
	{
		REPL,			//Command line interface
		Compile,		//Build
		CompileAndRun,	//Build and Execute
		RunOnly			//Execute
	};

	enum class CompilerBinaryType
	{
		AST, WCBytecode, x86, x64
	};

	struct CompilerProfile
	{
		CompilerProfile();

		CompilerAction Action;
		CompilerBinaryType BinaryType;

		std::string SourceFilepath, DestinationFilepath;

		bool VerboseOutput;

		bool BuildSymbolTable;
		std::string SymbolTableFilepath;
	};




}

#endif