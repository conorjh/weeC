#include "import.h"
#include <string>
#include <vector>
#include <fstream>

using namespace std;
using namespace wc;
using namespace wc::wcc;
using namespace wc::vm;
using namespace wc::comp;

namespace wc
{
	namespace wcc
	{
		//const char * wcc_exportDelimiter = ",";
	}
}

wcExecContext* wc::wcc::imp::importScriptFromFile(string p_fn)
{
	//attempt to load it
	vector<string>* source = loadFileAsStrings(getData()->origSource.c_str());
	if (source == nullptr)
	{
		//error loading source 
		return nullptr;
	}

	wcCompiler c(source);
	return nullptr;
}

wcExecContext* wc::wcc::imp::importByteCodeFromFile(string p_fn)
{

	return nullptr;
}