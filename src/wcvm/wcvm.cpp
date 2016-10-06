#include "wcvm.h"
#include "..\weec\export.h"
#include "..\weec\import.h"
#include <vector>
#include <string>
#include <iostream>

using namespace std;
using namespace wc;
using namespace wc::wcvm;


namespace wc
{
	namespace wcvm
	{
		wcvmData data;
		#include "test_scripts.h"	//std::vector<string> testScripts....
		const char *wcvmVer = "0.0.0.1";
	}
}

void wc::wcvm::execCmdLine()
{
	//run a file if given one from command line
	if (wcvm::getData()->runFile != "")
	{
		if (!wcvm::getData()->runFileIsTest)
			if (!wcvm::getData()->runFileIsByteCode)
				wcvm::getData()->mainContext.vm.con = imp::importScriptFromFile(wcvm::getData()->runFile);
			else
				wcvm::getData()->mainContext.vm.con = imp::importByteCodeFromFile(wcvm::getData()->runFile);
		else
			wcvm::getData()->mainContext.vm.con = wcvm::importTest(&comp::wcCompiler(), wcvm::getData()->runFile);
	}
}

void wc::wcvm::startup()
{
	getData()->displaySourceCode = true;
	getData()->inputModeOn = false;
	getData()->disableConsoleInput = false;
	getData()->runFile = "";
	getData()->runFileIsByteCode = false;
	getData()->runFileIsTest = false;
}

//returns the new index
int wc::wcvm::parseCmdLineArg(const char * p_args[], int p_i)
{
	int index = p_i;
	string arg(p_args[index]);

	//string literal, must be preceded by a command (eg   -c "filename.wcs")
	if (arg[0] == '"' && !index)
	{
		//opening string literal, trick parseCmdLineArg_Run into thinking
		//we've already had a -r or -run
		--index;
		return index = parseCmdLineArg_Run(p_args, index);
	}

	//compile the given source code
	if ((arg == "-r" || arg == "-run") && data.runFile == "")
	{
		//specify filename, next token should be a string literal
		return index = parseCmdLineArg_Run(p_args, index);
	}

	//disable the dos style console 
	if (arg == "-dc" || arg == "-disableconsole")
	{
		return index = parseCmdLineArg_DisableConsoleInput(p_args, index);
	}
	else if (data.runFile != "" && (arg == "-r" || arg == "-run"))
	{
		//error - we opened the cmdline with a string literal, but now want to issue another run command
		return index;
	}
	return ++index;	//unknown
}

//parse the -r or -run command
int wc::wcvm::parseCmdLineArg_Run(const char * p_args[], int p_i)
{
	//consume the -r or -run
	p_i++;
	string arg(p_args[p_i]);

	//expect only a file name in quotes
	if (arg[0] != '"')
	{
		//error
		return p_i;
	}
	else if (arg[arg.size() - 1] != '"')
	{
		//error - no closing quote
		return p_i;
	}
	else
	{
		//valid string format
		data.runFile = arg;
		if (util::getFileExt(arg) == ".wcs")
			data.runFileIsByteCode = false;
		else if (util::getFileExt(arg) == ".wc")
			data.runFileIsByteCode = true;
		else
			data.runFileIsByteCode = false;
	}

	//return index;
	return p_i;
}

int wc::wcvm::parseCmdLineArg_DisableConsoleInput(const char * p_args[], int p_i)
{
	p_i++;
	data.disableConsoleInput = true;
	return p_i;
}

wcvmData* wc::wcvm::getData()
{
	return &wc::wcvm::data;
}

string wc::wcvm::consoleGetInput()
{
	string in;
	cout << ">"; 
	getline(cin, in);
	return in;
}

int wc::wcvm::consoleParseCmd(string p_in)
{
	vector<string> tokens = wc::lex::tokenizeString(p_in);

	if (tokens.size() == 0)
		return 0;

	if (getData()->inputModeOn)
	{
		//add input to buffer
		if (p_in != "#end")
		{
			getData()->inputModeBuffer.push_back(p_in);
			return 1;
		}

		//exit command received, load the buffer into our source code buffer
		getData()->src = getData()->inputModeBuffer;
		getData()->inputModeBuffer.clear();
		getData()->inputModeOn = false;
		return 1;
	}

	if (tokens.at(0) == "run")
	{
		return consoleParseCmd_Run(tokens);
	}
	else if (tokens.at(0) == "runtest" || tokens.at(0) == "rt")
	{
		return consoleParseCmd_RunTest(tokens);
	}
	else if (tokens.at(0) == "help")
	{
		return consoleParseCmd_Help(tokens);
	}
	else if (tokens.at(0) == "#start" && !getData()->inputModeOn)
	{
		return consoleParseCmd_Input(tokens);
	}
	else 
	{
		//unknown command;
		cout << endl << "Unknown command: " << tokens.at(0) << endl;
		return 0;
	}
}

int wc::wcvm::consoleParseCmd_Run(vector<string> p_tok)
{
	if (p_tok.size() < 2)
		//no parameters
		return -1;

	getData()->runFile = p_tok[1];
	getData()->runFileIsTest = false;

	return 1;
}

int wc::wcvm::consoleParseCmd_RunTest(vector<string> p_tok)
{
	if (p_tok.size() < 2)
	{
		//no test specified
		return -1;
	}
	else
	{
		//check test exists
		int testInt = wc::util::wcstoi(p_tok.at(1));
		if (testInt < 0 || testInt >= testScripts.size())
			return -2;	//test doesnt exist

		//set our run file as the given test number
		getData()->runFile = p_tok.at(1);
	}

	getData()->runFileIsTest = true;
	return 1;
}

int wc::wcvm::consoleParseCmd_Help(vector<string> p_tok)
{
	if (p_tok.size() < 2)
		//display all commands
		return 0;
	else if (p_tok.at(1) == "run" || p_tok.at(1) == "runtest")
		//give help about acertain command
		return 1;
	else 
		//error unknown test
		return 2;

	getData()->runFileIsTest = true;

	return 1;
}

int wc::wcvm::consoleParseCmd_Input(vector<string> p_tok)
{
	getData()->inputModeOn = true;
	return 1;
}

int wc::wcvm::consoleLoop()
{
	wc::comp::wcCompiler c;

	//get input
	string in = consoleGetInput();
	
	//parse input
	if (consoleParseCmd(in) < 0)
		return 0;

	wcvmData* d = wcvm::getData();

	if (d->inputModeOn)
		return 1;

	//check file exists
	if (d->runFile != "")
	{
		if (!d->runFileIsTest)
			if (!d->runFileIsByteCode)
				d->mainContext.vm.con = imp::importScriptFromFile(d->runFile);
			else
				d->mainContext.vm.con = imp::importByteCodeFromFile(d->runFile);
		else
			d->mainContext.vm.con = wcvm::importTest(&c, d->runFile);
	}
	//no run file specified, but source code has been loaded from input mode
	else if (d->src.size())
	{		
		if (d->mainContext.vm.con)
			delete d->mainContext.vm.con;

	}
	else
	{
		return 1;
	}

	//print source
	
	if (d->displaySourceCode == true)
		for (int t = 0; t < d->src.size(); ++t)
			cout << d->src.at(t) << endl;
	cout << endl;
		
	//run
	run();

	//display output	
	wcError cErr = c.getError();
	if (cErr.code > 0)
	{
		cout << "Error "<< cErr.code << " @" << cErr.line << "," << cErr.column << "  " << comp::getCompilerErrorString(cErr.code) << " - " << cErr.msg << endl;
	}
	else
	{
		cout << "istream: " << d->mainContext.vm.con->istream.size() << endl;
		cout << "eax: " << d->mainContext.vm.con->reg[wc::vm::wcReg::eax] << endl;
		cout << "returns: " << d->mainContext.vm.con->reg[wc::vm::wcReg::ret] << endl;
		cout << endl;
	}
	
	return 1;
}

//load source code and also compile
wc::vm::wcExecContext* wc::wcvm::importTest(comp::wcCompiler* p_con, string p_t)
{
	getData()->src.clear(); 
	getData()->src.push_back(importTestAsString(p_t));
	
	//compile source to an execution context, check for errors
	return p_con->compile(&wc::wcvm::getData()->src);
}

string wc::wcvm::importTestAsString(string p_t)
{
	return testScripts[wc::util::wcstoi(p_t)];
}

vector<string> wc::wcvm::loadFileAsStrings(const char* p_f)
{
	vector<string> out;
	util::readFile(p_f, &out);
	return out;
}

void wc::wcvm::run()
{
	//run script until halt
	data.mainContext.vm.exec();
}

bool wc::wcvm::isRunning()
{
	return (!data.mainContext.vm.con->halt);
}