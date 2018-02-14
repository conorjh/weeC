#ifndef WASM_WASM_H
#define WASM_WASM_H
#include <vector>
#include <string>
#include <unordered_map>
#include "api.h"

namespace wasm
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

	struct wasmData
	{
		wasmData();
		std::string filenameSource;
	};

	//data
	extern wasmData data;

	void parseCmdLine(int argc, char *argv[]);
}
#endif