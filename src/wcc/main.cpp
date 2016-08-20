#include "wcc.h"
#include "test.h"
#include <iostream>

using namespace std;
using namespace wc::wcc;

int main(int argc, const char* args[])
{

	//inspect the command line
	int i = 0;
	while (i<argc)
		i = parseCmdLineArg(args, i);

	//wc::wcc::getData()->origSource = "test.wc";

	//compile source if we were given one
	compile();

	return 0;
}