#ifndef BCC_H
#define BCC_H

#include <string>
#include <vector>

#include "bcscript.h"
#include "export.h"

namespace bc
{
	namespace bcc
	{
		struct bccData
		{
			std::string destSource, destPCS, exportDelimiter;
		};

		void startup();
		void shutdown();

		int compile();
		bccData getData();

		std::vector<std::string>* loadSourceCode(const char*);

		void parseCmdLineArg(const char * args[], int p_i);
		int parseCmdLineArg_Compile(const char * args[], int p_i);

		void exportToByteCodeFile(vm::bcExecContext*,std::string);
	}
}

#endif