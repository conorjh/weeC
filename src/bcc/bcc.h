#ifndef BCC_H
#define BCC_H

#include <string>
#include <vector>
#include "..\bcscript\bcscript.h"
#include "export.h"

namespace bc
{
	namespace bcc
	{
		struct bccData
		{
			std::string origSource, compileTarget, exportDelimiter;
		};

		void startup();
		void shutdown();

		int compile();
		bccData* getData();

		std::vector<std::string>* loadFileAsStrings(const char*);

		int parseCmdLineArg(const char * args[], int p_i);
		int parseCmdLineArg_Compile(const char * args[], int p_i);
		int parseCmdLineArg_Target(const char * args[], int p_i);

		void exportToByteCodeFile(vm::bcExecContext*,std::string);
	}
}

#endif