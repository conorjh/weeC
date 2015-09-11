#ifndef BCVM_H
#define BCVM_H
#include <string>

namespace bc
{
	namespace bcvm
	{
		struct bcvmData
		{
			std::string runFile;
		};

		void parseCmdLineArg(const char * args[], int p_i);
		int parseCmdLineArg_Run(const char * args[], int p_i);

		void run();

		bcvmData* getData();
	}
}

#endif