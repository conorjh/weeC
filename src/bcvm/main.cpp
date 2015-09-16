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

	//do the command line
	bc::bcvm::execCmdLine();

	while (bcvm::isRunning())
		bcvm::run();

	return 0;
}
