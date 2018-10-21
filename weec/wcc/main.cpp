#include "wcc.h"

using namespace wcc;

int main_test_exportast(char *argv[]);

int main(int argc, char *argv[])
{
	return main_test_exportast(argv);

	init(argc,argv);

	//do
	exec();

	//exit
	cleanup();
	return 0;
}


int main_test_exportast(char *argv[])
{
	char *a[] = { argv[0], "-s", "exportast.wc", "-ast", "exportast.ast"};

	init(5, a);

	//do
	exec();

	//exit
	cleanup();
	return 0;
}