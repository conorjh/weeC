#ifndef BCVM_H
#define BCVM_H
#include <string>
#include "..\bcScript\bcscript.h"

namespace bc
{
	namespace bcvm
	{
		struct bcvmData
		{
			std::string runFile;
			bool runFileIsByteCode,displaySourceCode,runFileIsTest,disableConsoleInput,inputModeOn;
			vm::bcVM vm;
			std::vector<std::string> src;
			std::vector<std::string> inputModeBuffer;
		};

		void startup();

		int parseCmdLineArg(const char * args[], int p_i);
		int parseCmdLineArg_Run(const char * args[], int p_i);
		int parseCmdLineArg_DisableConsoleInput(const char * args[], int p_i);
		void execCmdLine();

		void run();
		int consoleLoop();
		std::string consoleGetInput();
		int consoleParseCmd(std::string);
		int consoleParseCmd_Run(std::vector<std::string>);
		int consoleParseCmd_RunTest(std::vector<std::string>);
		int consoleParseCmd_Input(std::vector<std::string>);

		bc::vm::bcExecContext* importTest(std::string);
		std::string importTestAsString(std::string);
		std::vector<std::string> loadFileAsStrings(const char* p_f);
		bool isRunning();

		bcvmData* getData();
	}
}

#endif