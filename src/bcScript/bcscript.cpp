#include "compiler.h"
#include "bcscript.h"
#include <iostream>

using namespace std;
using namespace bc;
using namespace bc::comp;
using namespace bc::vm;
using namespace bc::imp;
using namespace bc::parse;

//load script
namespace bc
{
	bcScript* loadScriptFromFile(bcContext*, std::string);
	bcScript* loadScriptFromString(bcContext*, std::string);
}

void bc::bcScript::clear()
{
	con->clear();
	isExecuted = isCompiled = false;
	src.clear();
}

int bc::bcScript::load(string p_str)
{
	*this = imp::importScriptFromStringToScript(p_str);	
	return 1;
}

int bc::bcScript::load(vector<string>* p_vec)
{
	*this = imp::importScriptFromVectorToScript(*p_vec);
	return 1;
}

int bc::bcScript::loadFromFile(string p_file)
{
	bool isBytecode = false;

	//check whether extension is script or bytecode
	if (util::getFileExt(p_file) == ".bc")
		isBytecode = true;

	//check whether file exists
	if (!util::fileExists(p_file.c_str()))
		return -1;

	//load file
	if (isBytecode)
		*this = importByteCodeFromFileToScript(p_file);
	else
		*this = importScriptFromFileToScript(p_file);

	//check for errors
	if (con == nullptr)
		return -1;

	return 1;
}

int bc::startup(bcContext* p_c)
{
	//set defaults
	p_c->data.id = rand();	//random number here...
	p_c->data.alwaysAttachScriptOnRun = false;

	return 0;
}

int bc::shutdown(bcContext* p_c)
{

	return 0;
}

int bc::compileScript(bcScript *p_scr)
{
	bcCompiler c;
	if (c.compile(&p_scr->src) < 0)
		return -1;
	p_scr->con = c.output;
	p_scr->isCompiled = true;
	return 1;
}

bcScript* bc::loadScriptFromFile(bcContext *p_ec, string p_file)
{
	p_ec->scr.clear();
	p_ec->scr = importScriptFromFileToScript(p_file);
	return &p_ec->scr;
}

bcScript* bc::loadScriptFromString(bcContext *p_ec, string p_str)
{
	p_ec->scr.clear();
	p_ec->scr = importScriptFromStringToScript(p_str);	
	return &p_ec->scr;
}

int bc::run(bcContext* p_con, bcScript* p_scr)
{
	//compile if needed
	if (!p_scr->isCompiled)
		if (compileScript(p_scr) < 0)
		{
			//compile error			
			return -1;
		}

	//attach script to context if needed
	if(p_con->data.alwaysAttachScriptOnRun)
		if (&p_con->scr != p_scr)
			p_con->scr = *p_scr;

	//run bytecode
	p_con->vm.con = p_scr->con;
	p_con->vm.exec(0);
	p_con->scr.isExecuted = true;

	return 1;
}

int bc::run(bcContext *p_con, vm::bcExecContext *p_econ)
{
	//run given bytecode
	p_con->vm.con = p_econ;
	p_con->vm.exec(0);
	p_con->scr.isExecuted = true;

	return 1;
}


int bc::runScriptFromFile(bcContext* p_c, std::string p_f)
{
	//load from file
	p_c->scr = *loadScriptFromFile(p_c,p_f);

	//run bytecode
	p_c->vm.con = p_c->scr.con;
	p_c->vm.exec(0);
	p_c->scr.isExecuted = true;

	return 0;
}

//p_sym should be a full ident ie "$global::main::foo" not "foo"
bool bc::doesSymbolExist(bcScript *p_scr, string p_sym)
{
	bcExecContext* ec = p_scr->con;
	
	if(!p_scr->isCompiled || !p_scr->isExecuted)
		return false;	//uncompiled, or unexecuted
	if (ec->symTab.find(p_sym) == ec->symTab.end())
		return false;	//symbol not found in symbol table (should be all symbols in the script)
	if (!doesStoredFrameExist(p_scr,ec->symTab[p_sym].stackFrame))
		return false;	//script has ran, but the function in which the symbol existed was never ran
	
	//must exist
	return true;
}

bcVMSymbol bc::getSymbol(bcScript *p_scr, string p_sym)
{
	//check symbol exists
	if (!doesSymbolExist(p_scr, p_sym))
		return bcVMSymbol();
	return p_scr->con->symTab[p_sym];
}

int bc::getSymbolValue(bcScript *p_scr, string p_sym)
{
	if (!doesSymbolExist(p_scr, p_sym))
		return 0;
	bcVMSymbol sym = getSymbol(p_scr, p_sym);	
	return *p_scr->con->storedFrames[sym.stackFrame].data.at(sym.offset);
}

bool bc::doesStoredFrameExist(bcScript *p_scr, int p_sid)
{
	if (p_scr->con->storedFrames.find(p_sid) == p_scr->con->storedFrames.end())
		return false;	//script has ran, but the function in which the symbol existed was never ran
	return true;
}


