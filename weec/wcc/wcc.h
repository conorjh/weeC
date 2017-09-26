#ifndef WCC_WCC_H
#define WCC_WCC_H
#include <vector>
#include "io.h"

namespace wcc
{
	void exec();
	void init(int argc, char *argv[]);
	void cleanup();

	enum CmdLineArgType
	{
		null, command, optional, free_floating_parameter
	};

	struct CmdLineArg
	{
		CmdLineArg();
		CmdLineArgType type;
		std::string cmdlet;
		std::vector<std::string> params;
	};

	struct CompilerOutput
	{
		wc::compile::wcBaseCompiler* compiler;
		wc::api::wcScript script;
	};

	enum SourceType
	{
		wcst_sourcecode, wcst_ast, wcst_bytecode, wcst_bytecode_debug,
	};

	const std::unordered_multimap<int, std::string> sourceTypeStrings =
	{
		{ wcst_sourcecode,"Source Code" },
		{ wcst_ast,"AST" },
		{ wcst_bytecode,"Precompiled Bytecode" },
		{ wcst_bytecode_debug,"Precompiled Bytecode with Debug Info" }
	};


	enum CompileTarget
	{
		wcct_bytecode, wcct_simple_bytecode, wcct_x86, wcct_ansi_c
	};

	const std::unordered_multimap<int, std::string> targetStrings =
	{
		{ wcct_bytecode,"Bytecode" },
		{ wcct_simple_bytecode,"Simple Bytecode" },
		{ wcct_x86,"x86" },
		{ wcct_ansi_c,"Ansi C" }
	};

	struct wccData
	{
		wccData();
		std::string filenameSource, filenameOutput, filenameAST;
		SourceType sourceType;
		CompileTarget target;
		bool consoleOn, displayOutput, outputAST;
	};

	//data
	extern wccData data;

	void parseCmdLine(int argc, char *argv[]);
}

#endif 

