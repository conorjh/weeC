#include "wcvm.h"

using namespace wcvm;

int main(int argc, char *argv[])
{
	init(argc, argv);

	//do
	exec();

	//exit
	cleanup();
	return 0;
}