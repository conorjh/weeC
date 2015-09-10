#include "bcc.h"
#include "test.h"
#include <iostream>

using namespace std;

int main(int argc, const char* args[])
{
	//execute command line args
	for (int t = 0; t < argc; ++t)
		bc::bcc::parseCmdLineArg(args,t);

	//compile source if we were given one
	bc::bcc::compile();

	return 0;
}