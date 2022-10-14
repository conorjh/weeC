#include "util.h"
#include <iostream>
#include <string>
#include <sstream>

int weec::util::stoi(std::string Input)
{
    std::stringstream ss;
    int Output;
    ss << Input;
    ss >> Output;
    return Output;
}

std::string weec::util::itos(int)
{
    return std::string();
}
