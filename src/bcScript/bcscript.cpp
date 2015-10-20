#include "bcscript.h"
#include <iostream>

using namespace std;
using namespace bc;

int bc::startup(bcContext* p_c)
{

	return 0;
}

int bc::shutdown(bcContext* p_c)
{

	return 0;
}

bcScript bc::loadScriptFromFile(bcContext *p_ec, string p_file)
{
	bcScript s;
	
	bool isBytecode = false;
	
	//check whether extension is script or bytecode
	if (util::getFileExt(p_file) == ".bc")
		isBytecode = true;
	
	//check whether file exists
	if (!util::fileExists(p_file.c_str()))
		return s;
	
	//load file
	if (isBytecode)
	{
		s.con = imp::importByteCodeFromFile(p_file);
	}
	else
	{
		s.con = imp::importScriptFromFile(p_file);
	}
	//check for errors

	return s;
}

bcScript bc::loadScriptFromString(bcContext *p_ec, string p_str)
{
	return bcScript();
}

int bc::runScriptFromFile()
{
	return 0;
}

int bc::bcContext::loadScript(bcScript)
{
	return 0;
}
