#include "wcc.h"
//#include "test.h"
#include <iostream>

using namespace std;
using namespace wc::wcc;

int main(int argc, const char* args[])
{
	startup();

	//inspect the command line
	int i = 0;
	while (i<argc)
		i = parseCmdLineArg(args, i);
	
	//compile source if we were given one
	compile();

	shutdown();

	return 0;
}