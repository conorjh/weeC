#include "util.h"
#include <fstream>
#include <sstream>

using namespace std;
using namespace wc::util;

bool wc::util::fileExists(string p_filename)
{
	std::ifstream ifile(p_filename.c_str());
	return (bool)ifile;
}

vector<string> wc::util::loadFileAsText(string p_filename)
{
	vector<string> output;

	//chcek file exists
	if (!fileExists(p_filename))
		return output;

	//load each line as a string
	string lineBuffer;
	ifstream myfile(p_filename);
	while (getline(myfile, lineBuffer))
		output.push_back(lineBuffer);

	//return output
	return output;
}

int wc::util::randomInt()
{
	int min = INT_MIN;
	int max = INT_MAX;
	return min + rand() % (static_cast<int>(max - min));
}

unsigned int wc::util::randomUInt()
{
	unsigned int min = 0;
	unsigned int max = UINT_MAX;
	return min + (rand() % static_cast<unsigned int>(max - min));
}

int wc::util::stoi(string p_in)
{
	stringstream ss(p_in);
	int out;
	ss >> out;
	return out;
}

int wc::util::stoi(char p_in)
{
	stringstream ss(p_in);
	int out;
	ss >> out;
	return out;
}

string wc::util::itos(int p_in)
{
	stringstream ss;
	ss << p_in;
	return ss.str();
}

std::string wc::util::ftos(float p_f)
{
	int nd = 2;
	ostringstream ostr;
	int tens = std::stoi("1" + string(nd, '0'));
	ostr << round(p_f*tens) / tens;
	return ostr.str();
}
