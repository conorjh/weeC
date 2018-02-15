#ifndef WCVM_WCVM_H
#define WCVM_WCVM_H
#include <vector>
#include <string>
#include <unordered_map>
#include "api.h"

namespace wcvm
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

	enum SourceType
	{
		wcvmst_bytecode, wcvmst_simple_bytecode
	};

	struct wcvmData
	{
		wcvmData();
		std::string filenameSource;
		SourceType sourceType;
	};

	//data
	extern wcvmData data;

	void parseCmdLine(int argc, char *argv[]);
	bool parseCmdLine_Arg(CmdLineArg p_arg);
	std::vector<CmdLineArg> convertCmdLineArgs(int p_argc, char *p_argv[]);

}
#endif