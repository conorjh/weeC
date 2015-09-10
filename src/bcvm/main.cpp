#include "bcvm.h"
#include <iostream>

using namespace std;

int main(int argc, const char* args[])
{
	//execute command line args
	for (int t = 0; t < argc; ++t)
		bc::bcvm::parseCmdLineArg(args, t);

	//compile source if we were given one
	bc::bcvm::run();

	return 0;
}
