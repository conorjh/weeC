#ifndef WCVM_H
#define WCVM_H
#include <string>
#include <vector>
#include "..\weec\weec.h"

namespace wc
{
	namespace wcvm
	{
		extern const char * wcvmVer;
		struct wcvmData
		{
			std::string runFile;
			bool runFileIsByteCode,displaySourceCode,runFileIsTest,disableConsoleInput,inputModeOn;
			vm::wcVM vm;
			std::vector<std::string> src;
			std::vector<std::string> inputModeBuffer;
		};

		extern std::vector<std::string> testScripts; //script id to actual script code

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
		int consoleParseCmd_Help(std::vector<std::string>);
		int consoleParseCmd_Input(std::vector<std::string>);

		wc::vm::wcExecContext* importTest(comp::wcCompiler* p_con,std::string);
		std::string importTestAsString(std::string);
		std::vector<std::string> loadFileAsStrings(const char* p_f);
		bool isRunning();

		wcvmData* getData();
	}
}

#endif