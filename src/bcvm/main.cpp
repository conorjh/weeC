#include "bcvm.h"
#include <iostream>

using namespace std;
using namespace bc;

int main(int argc, const char* args[])
{
	bcvm::startup();

	//inspect the command line
	int i = 0;
	while (i<argc)
		i = bc::bcvm::parseCmdLineArg(args, i);

	//do the command line
	bc::bcvm::execCmdLine();

	//console loop
	if (!bcvm::getData()->disableConsoleInput)
		while (bcvm::consoleLoop())
			true;
	else
		while (bcvm::isRunning())
			bcvm::run();

	//cleanup


	//exit
	return 0;
}
