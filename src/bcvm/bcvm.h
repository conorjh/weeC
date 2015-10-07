#ifndef BCVM_BCVM_H
#define BCVM_BCVM_H
#include <string>
#include "..\bcScript\bcscript.h"

namespace bc
{
	namespace bcvm
	{
		struct bcvmData
		{
			std::string runFile;
			bc::vm::bcVM vm;
		};

		int parseCmdLineArg(const char * args[], int p_i);
		int parseCmdLineArg_Run(const char * args[], int p_i);
		void execCmdLine();

		void run();
		bool isRunning();

		bcvmData* getData();
	}
}

#endif