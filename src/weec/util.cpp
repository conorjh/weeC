#include "util.h"
#include <fstream>
#include "lexer.h"

using namespace std;
using namespace wc;
using namespace wc::lex;
using wc::lex::wcTokenType;

string wc::util::wcitos(int in)
{
	string s;
	stringstream out;
	out << in;
	return out.str();
}


int wc::util::wcstoi(string in)
{
	int retval;
	stringstream convert(in);
	if (!(convert >> retval))
		return 0;
	else
		return retval;
}

string wc::util::wcftos(float in)
{
	string s;
	stringstream out;
	out << in;
	return out.str();
}


float wc::util::wcstof(string in)
{
	float retval;
	stringstream convert(in);
	if (!(convert >> retval))
		return 0;
	else
		return retval;
}

string wc::util::getFileExt(string p_filename)
{
	int index = p_filename.size() - 1;

	while (index != 0)
		if (p_filename.at(index) == '.')
			return p_filename.substr(index, p_filename.size() - 1);
		else
			index--;
	return "";
}

string wc::util::toLower(string in)
{
	for (int i = 0; i<strlen(in.c_str()); i++)
		if (in[i] >= 0x41 && in[i] <= 0x5A)
			in[i] = in[i] + 0x20;
	return in;
}

string wc::util::toLower(char in)
{
	string out;
	if (in >= 0x41 && in <= 0x5A)
		in = in + 0x20;
	out += in;
	return out;
}

bool wc::util::fileExists(const char *filename)
{
	ifstream ifile(filename);
	return ifile.good();
}

bool wc::util::readFile(const char* in, vector<string>* out)
{
	if (!fileExists(in))
		return false;

	string line;
	ifstream myfile(in);

	if (myfile.is_open())
	{
		while (myfile.good())
		{
			getline(myfile, line);
			line.append("\n");
			out->push_back(line);
		}
		myfile.close();
		return true;
	}
	else
	{
		return false;
	}
}
