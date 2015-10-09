#include "bcvm.h"
#include <iostream>

using namespace std;
using namespace bc;

int main(int argc, const char* args[])
{
	//inspect the command line
	int i = 0;
	while (i<argc)
		i = bc::bcvm::parseCmdLineArg(args, i);

	bcvm::getData()->runFile = "../scripts/test.bcs";
	bcvm::getData()->runFileIsByteCode = false;

	//do the command line
	bc::bcvm::execCmdLine();

	//console loop
	if (!bcvm::getData()->disableConsoleInput)
		bcvm::consoleLoop();
	else
		while (bcvm::isRunning())
			bcvm::run();

	//cleanup


	//exit
	return 0;
}
