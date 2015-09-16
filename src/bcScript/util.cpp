#include "util.h"
#include <fstream>
#include "lexer.h"

using namespace std;
using namespace bc;
using namespace bc::lex;
using bc::lex::bcTokenType;

std::string bc::util::bcitos(int in)
{
	std::string s;
	std::stringstream out;
	out << in;
	return out.str();
}
	

int bc::util::bcstoi(std::string in)
{	
	int retval;
	std::stringstream convert(in);
	if ( !(convert >> retval) )
		return 0;
	else
		return retval;
}

std::string bc::util::bcftos(float in)
{
	std::string s;
	std::stringstream out;
	out << in;
	return out.str();
}
	

float bc::util::bcstof(std::string in)
{	
	float retval;
	std::stringstream convert(in);
	if ( !(convert >> retval) )
		return 0;
	else
		return retval;
}

string bc::util::getFileExt(string p_filename)
{
	int index = p_filename.size() - 1;

	while (index != 0)
		if (p_filename.at(index) == '.')
			return p_filename.substr(index, p_filename.size() - 1);
		else
			index--;
	return "";
}

std::string bc::util::toLower(std::string in)
{
	for (int i=0;i<strlen(in.c_str());i++)
	if (in[i] >= 0x41 && in[i] <= 0x5A)
		in[i] = in[i] + 0x20;
	return in;
}

std::string bc::util::toLower(char in)
{
	std::string out;
	if (in >= 0x41 && in <= 0x5A)
		in = in + 0x20;
	out+=in;
	return out;
}

bool bc::util::fileExists(const char *filename)
{
	std::ifstream ifile(filename);
	return ifile.good();
}

bool bc::util::readFile(const char* in,std::vector<std::string>* out)
{
	if (!fileExists(in))
		return false;

	std::string line;
	std::ifstream myfile(in);
  
	if (myfile.is_open())
	{
		while ( myfile.good() )
		{
			getline(myfile,line);
			line.append("\n");
			out->push_back( line );		
		}
		myfile.close();
		return true;
	}
	else
	{
		return false;
	}
}
