#include "import.h"
#include "weec.h"
#include <string>
#include <vector>
#include <fstream>

using namespace std;
using namespace wc;
using namespace wc::vm;
using namespace wc::comp;

namespace wc
{
	namespace wcc
	{
		//const char * wcc_exportDelimiter = ",";
	}
}

wcExecContext* wc::imp::importScriptFromFile(string p_fn)
{
	//attempt to load it
	vector<string>* source = new vector<string>();
	if(util::readFile(p_fn.c_str(), source))
	{
		//error loading source 
		return nullptr;
	}

	//compile it 
	wcCompiler c(source);
	delete source;

	//return it
	return c.output;
}

wcExecContext* wc::imp::importByteCodeFromFile(string p_fn)
{

	return nullptr;
}

wcScript wc::imp::importScriptFromFileToScript(string p_fn)
{
	wcScript s;

	//attempt to load it
	if (util::readFile(p_fn.c_str(), &s.src))
	{
		//error loading source 
		return s;
	}

	//compile it 
	wcCompiler c(&s.src);
	s.con = c.output;
	s.isCompiled = true;
	
	//return it
	return s;
}

wc::wcScript wc::imp::importScriptFromVectorToScript(std::vector<std::string> p_vec)
{
	wcScript s;
	s.src = p_vec;

	//compile it 
	wcCompiler c(&s.src);
	s.con = c.output;
	s.isCompiled = true;

	//return it
	return s;
}

wcScript wc::imp::importByteCodeFromFileToScript(string p_fn)
{

	return wcScript();
}

//loads a string into a script, 
wc::wcScript wc::imp::importScriptFromStringToScript(string p_str)
{
	return importScriptFromVectorToScript(vector<string>({ p_str }));
}
