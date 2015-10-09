#include "import.h"
#include "bcscript.h"
#include <string>
#include <vector>
#include <fstream>

using namespace std;
using namespace bc;
using namespace bc::vm;
using namespace bc::comp;

namespace bc
{
	namespace bcc
	{
		//const char * bcc_exportDelimiter = ",";
	}
}

bcExecContext* bc::imp::importScriptFromFile(string p_fn)
{
	//attempt to load it
	vector<string>* source = new vector<string>();
	if(util::readFile(p_fn.c_str(), source))
	{
		//error loading source 
		return nullptr;
	}

	//compile it 
	bcCompiler c(source);

	//return it
	return c.output;
}

bcExecContext* bc::imp::importByteCodeFromFile(string p_fn)
{

	return nullptr;
}