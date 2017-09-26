#include "wcc.h"

using namespace wcc;

int main(int argc, char *argv[])
{
	init(argc,argv);

	//do
	exec();

	//exit
	cleanup();
	return 0;
}