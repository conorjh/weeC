#include "bcc.h"
#include "test.h"
#include <iostream>

using namespace std;
using namespace bc::bcc;

int main(int argc, const char* args[])
{

	//inspect the command line
	int i = 0;
	while (i<argc)
		i = parseCmdLineArg(args, i);

	//bc::bcc::getData()->origSource = "test.bc";

	//compile source if we were given one
	compile();

	return 0;
}