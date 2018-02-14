#include "wasm.h"

using namespace wasm;

int main(int argc, char *argv[])
{
	init(argc, argv);

	//do
	exec();

	//exit
	cleanup();
	return 0;
}