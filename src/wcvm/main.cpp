#include "wcvm.h"
#include <iostream>

using namespace std;
using namespace wc;

int main(int argc, const char* args[])
{
	wcvm::startup();

	//inspect and execute the command line
	int i = 0;
	while (i<argc)
		i = wc::wcvm::parseCmdLineArg(args, i);
	wc::wcvm::execCmdLine();

	//splash
	cout << "weeC Virtual Machine	v" << wcvm::wcvmVer 
	<< std::endl << "- - - - - -" << std::endl;

	//console loop
	if (!wcvm::getData()->disableConsoleInput)
		while (wcvm::consoleLoop())
			true;
	else
		while (wcvm::isRunning())
			wcvm::run();

	//cleanup


	//exit
	return 0;
}
