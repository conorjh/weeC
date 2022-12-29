#include "Util.h"
#include "App.h"
#include <fstream>
#include <iostream>

using namespace std;
using namespace App;

bool Util::FileExists(std::string Filepath)
{
	ifstream ifile;
	ifile.open(Filepath);
	if (ifile)
		return true;
	else
		return false;
}

std::string  Util::itos(int i)
{
	return "";
}

std::string  Util::uitos(unsigned int i)
{
	return "";
}

