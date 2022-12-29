#include "App.h"

using namespace App;

int main(int argc, char* argv[])
{
	Application App(argc, argv);

	if (!App.Init())
		return 0;

	while (!App.Ended())
	{
		App.Update();
	}

	return 0;
}
