#include <string>
#include "api.h"
#include "util.h"
#include "compiler.h"

using namespace std;
using namespace wc;
using namespace wc::api;
using namespace wc::compile;
using namespace wc::bytecode;

namespace wc
{
	namespace api
	{

	}
}

wc::api::wcScript::wcScript()
{
	
	this->handle = 0;
}

wc::api::wcScript::wcScript(wcError p_error)
{
	error = p_error;
}

wcError wc::api::wcScript::getError()
{
	return error;
}

void wc::api::wcScript::setError(wcError p_error)
{
	error = p_error;
}

wcScript wc::api::loadScriptFromFile(string p_filename)
{
	//look for input file
	if (!util::fileExists(p_filename))
		return wcScript();
	vector<string> source = util::loadFileAsText(p_filename);

	//attempt to compile
	wcClassicCompiler comp;
	wcScript output = comp.compile(source);
	if (comp.getError().code)
		return wcScript(comp.getError());

	return output;
}
