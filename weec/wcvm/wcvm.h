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

	struct wcvmData
	{
		wcvmData();
		std::string filenameSource;
	};

	//data
	extern wcvmData data;

	void parseCmdLine(int argc, char *argv[]);
}



#endif