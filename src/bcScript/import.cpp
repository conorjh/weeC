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

bcScript bc::imp::importScriptFromFileToScript(string p_fn)
{
	bcScript s;
	//attempt to load it
	if (util::readFile(p_fn.c_str(), &s.src))
	{
		//error loading source 
		return s;
	}

	//compile it 
	bcCompiler c(&s.src);
	s.con->istream = *c.g.istream;

	//return it
	return s;
}

bcScript bc::imp::importByteCodeFromFileToScript(string p_fn)
{

	return bcScript();
}