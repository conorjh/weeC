#include "compiler.h"
#include "weec.h"
#include <iostream>

using namespace std;
using namespace wc;
using namespace wc::comp;
using namespace wc::vm;
using namespace wc::imp;
using namespace wc::parse;

//load script
namespace wc
{
	wcScript* loadScriptFromFile(wcContext*, std::string);
	wcScript* loadScriptFromString(wcContext*, std::string);
}

void wc::wcScript::clear()
{
	con->clear();
	isExecuted = isCompiled = false;
	src.clear();
}

int wc::wcScript::load(string p_str)
{
	*this = imp::importScriptFromStringToScript(p_str);
	return 1;
}

int wc::wcScript::load(vector<string>* p_vec)
{
	*this = imp::importScriptFromVectorToScript(*p_vec);
	return 1;
}

int wc::wcScript::loadFromFile(string p_file)
{
	bool isBytecode = false;

	//check whether extension is script or bytecode
	if (util::getFileExt(p_file) == ".wc")
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

int wc::startup(wcContext* p_c)
{
	//set defaults
	p_c->data.id = rand();	//random number here...
	p_c->data.alwaysAttachScriptOnRun = false;

	return 0;
}

int wc::shutdown(wcContext* p_c)
{

	return 0;
}

int wc::compileScript(wcScript *p_scr)
{
	wcCompiler c;
	if (c.compile(&p_scr->src) < 0)
		return c.getError();
	p_scr->con = c.output;
	p_scr->isCompiled = true;
	return 1;
}

wcScript* wc::loadScriptFromFile(wcContext *p_ec, string p_file)
{
	p_ec->scr.clear();
	p_ec->scr = importScriptFromFileToScript(p_file);
	return &p_ec->scr;
}

wcScript* wc::loadScriptFromString(wcContext *p_ec, string p_str)
{
	p_ec->scr.clear();
	p_ec->scr = importScriptFromStringToScript(p_str);
	return &p_ec->scr;
}

int wc::run(wcContext* p_con, wcScript* p_scr)
{
	//compile if needed
	if (!p_scr->isCompiled)
		if (compileScript(p_scr) < 0)
		{
			//compile error			
			return -1;
		}

	//attach script to context if needed
	if (p_con->data.alwaysAttachScriptOnRun)
		if (&p_con->scr != p_scr)
			p_con->scr = *p_scr;

	//run bytecode
	p_con->vm.con = p_scr->con;
	p_con->vm.exec(0);
	p_con->scr.isExecuted = true;

	return 1;
}

int wc::run(wcContext *p_con, vm::wcExecContext *p_econ)
{
	//run given bytecode
	p_con->vm.con = p_econ;
	p_con->vm.exec(0);
	p_con->scr.isExecuted = true;

	return 1;
}


int wc::runScriptFromFile(wcContext* p_c, std::string p_f)
{
	//load from file
	p_c->scr = *loadScriptFromFile(p_c, p_f);

	//run bytecode
	p_c->vm.con = p_c->scr.con;
	p_c->vm.exec(0);
	p_c->scr.isExecuted = true;

	return 0;
}

//p_sym should be a full ident ie "$global::main::foo" not "foo"
bool wc::doesSymbolExist(wcScript *p_scr, string p_sym)
{
	wcExecContext* ec = p_scr->con;

	if (!p_scr->isCompiled || !p_scr->isExecuted)
		return false;	//uncompiled, or unexecuted
	if (ec->symTab.find(p_sym) == ec->symTab.end())
		return false;	//symbol not found in symbol table (should be all symbols in the script)
	if (!doesStoredFrameExist(p_scr, ec->symTab[p_sym].stackFrame))
		return false;	//script has ran, but the function in which the symbol existed was never ran

						//must exist
	return true;
}

wcVMSymbol wc::getSymbol(wcScript *p_scr, string p_sym)
{
	//check symbol exists
	if (!doesSymbolExist(p_scr, p_sym))
		return wcVMSymbol();
	return p_scr->con->symTab[p_sym];
}

int wc::getSymbolValue(wcScript *p_scr, string p_sym)
{
	if (!doesSymbolExist(p_scr, p_sym))
		return 0;
	wcVMSymbol sym = getSymbol(p_scr, p_sym);
	return *p_scr->con->storedFrames[sym.stackFrame].data.at(sym.offset);
}

bool wc::doesStoredFrameExist(wcScript *p_scr, int p_sid)
{
	if (p_scr->con->storedFrames.find(p_sid) == p_scr->con->storedFrames.end())
		return false;	//script has ran, but the function in which the symbol existed was never ran
	return true;
}


