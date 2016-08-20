#ifndef WEEC_WCC_H
#define WEEC_WCC_H

#include <string>
#include <vector>
#include "..\weec\weec.h"

namespace wc
{
	namespace wcc
	{
		struct wccData
		{
			std::string origSource, compileTarget, exportDelimiter;
		};

		void startup();
		void shutdown();

		int compile();
		wccData* getData();

		std::vector<std::string>* loadFileAsStrings(const char*);

		int parseCmdLineArg(const char * args[], int p_i);
		int parseCmdLineArg_Compile(const char * args[], int p_i);
		int parseCmdLineArg_Target(const char * args[], int p_i);

		void exportToByteCodeFile(vm::wcExecContext*,std::string);
		bool isValidScriptFilename(std::string);
	}
}

#endif