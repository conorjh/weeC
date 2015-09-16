#include "import.h"
#include <string>
#include <vector>
#include <fstream>

using namespace std;
using namespace bc;
using namespace bc::bcc;
using namespace bc::vm;
using namespace bc::comp;

namespace bc
{
	namespace bcc
	{
		//const char * bcc_exportDelimiter = ",";
	}
}

bcExecContext* bc::bcc::imp::importScriptFromFile(string p_fn)
{
	//attempt to load it
	vector<string>* source = loadFileAsStrings(getData()->origSource.c_str());
	if (source == nullptr)
	{
		//error loading source 
		return nullptr;
	}

	bcCompiler c(source);
	return nullptr;
}

bcExecContext* bc::bcc::imp::importByteCodeFromFile(string p_fn)
{

	return nullptr;
}