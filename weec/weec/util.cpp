#include "util.h"
#include <iostream>
#include <string>
#include <sstream>
using namespace std;

string weec::util::AnyToString(any In)
{
	if (strcmp(In.type().name(), "int") == 0)
		return to_string(any_cast<int>(In));
	else if (strcmp(In.type().name(), "unsigned int") == 0)
		return to_string(any_cast<unsigned int>(In));
	else if (strcmp(In.type().name(), "float") == 0)
		return to_string(any_cast<float>(In));
	else if (strcmp(In.type().name(), "double") == 0)
		return to_string(any_cast<double>(In));
	else if (strcmp(In.type().name(), "bool") == 0)
		return to_string(any_cast<bool>(In));
	return "";
}

