#include "bcvm.h"
#include "..\bcc\bcc.h"
#include "..\bcScript\export.h"
#include "..\bcScript\import.h"
#include <vector>
#include <string>
#include <iostream>

using namespace std;
using namespace bc;
using namespace bc::bcvm;


namespace bc
{
	namespace bcvm
	{
		bcvmData data;
	}
}

void bc::bcvm::execCmdLine()
{
	getData()->displaySourceCode = true;

	//run a file if given one from command line
	if (bcvm::getData()->runFile != "")
		if (!bcvm::getData()->runFileIsTest)
			if (!bcvm::getData()->runFileIsByteCode)
				bcvm::getData()->vm.con = bc::imp::importScriptFromFile(bcvm::getData()->runFile);
			else
				bcvm::getData()->vm.con = bc::imp::importByteCodeFromFile(bcvm::getData()->runFile);
		else
			bcvm::getData()->vm.con = bcvm::importTest(bcvm::getData()->runFile);	
}

//returns the new index
int bc::bcvm::parseCmdLineArg(const char * p_args[], int p_i)
{
	int index = p_i;
	string arg(p_args[index]);

	//string literal, must be preceded by a command (eg   -c "filename.bcs")
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
		//error - we opened the cmdline with a string literal, but now want to issue anothe run command
		return index;
	}
	return ++index;	//unknown
}

//parse the -r or -run command
int bc::bcvm::parseCmdLineArg_Run(const char * p_args[], int p_i)
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
		if (util::getFileExt(arg) == ".bcs")
			data.runFileIsByteCode = false;
		else if (util::getFileExt(arg) == ".bc")
			data.runFileIsByteCode = true;
		else
			data.runFileIsByteCode = false;
	}

	//return index;
	return p_i;
}

int bc::bcvm::parseCmdLineArg_DisableConsoleInput(const char * p_args[], int p_i)
{
	p_i++;
	data.disableConsoleInput = true;
	return p_i;
}

bcvmData* bc::bcvm::getData()
{
	return &bc::bcvm::data;
}

string bc::bcvm::consoleGetInput()
{
	string in;
	cout << ">"; 
	getline(cin, in);
	return in;
}

int bc::bcvm::consoleParseCmd(string p_in)
{
	vector<string> tokens = bc::lex::tokenizeString(p_in);

	if (tokens.size() == 0)
		return 0;

	if (tokens.at(0) == "run")
	{
		return consoleParseCmd_Run(tokens);
	}
	else if (tokens.at(0) == "runtest")
	{
		return consoleParseCmd_RunTest(tokens);
	}
	else 
	{
		//unknown command;
		cout << endl << "Unknown command: " << tokens.at(0) << endl;
		return -1;
	}
}

int bc::bcvm::consoleParseCmd_Run(vector<string> p_tok)
{
	if (p_tok.size() < 2)
		//no parameters
		return -1;

	getData()->runFile = p_tok[1];
	getData()->runFileIsTest = false;

	return 1;
}

int bc::bcvm::consoleParseCmd_RunTest(vector<string> p_tok)
{
	if (p_tok.size() < 2)
		//no test specified
		return -1;
	else if (p_tok.at(1) == "1" || p_tok.at(1) == "2" || p_tok.at(1) == "3")
		//set our run file as the given test number
		getData()->runFile = p_tok.at(1);
	else
		//error unknown test
		return -2;

	getData()->runFileIsTest = true;

	return 1;
}

int bc::bcvm::consoleLoop()
{
	//get input
	string in = consoleGetInput();
	
	//parse input
	if (consoleParseCmd(in) < 0)
		return 0;

	//check file exists
	bcvmData* d = bcvm::getData();
	if (d->runFile != "")
		if (!d->runFileIsTest)
			if (!d->runFileIsByteCode)
				d->vm.con = imp::importScriptFromFile(d->runFile);
			else
				d->vm.con = imp::importByteCodeFromFile(d->runFile);
		else
			d->vm.con = bcvm::importTest(d->runFile);
	
	//print source
	if (d->displaySourceCode == true)
	{
		for (int t = 0; t < d->src.size(); ++t)
			cout << d->src.at(t);
	}
	cout << endl;

	//run
	run();

	cout << endl;
	return 1;
}

bc::vm::bcExecContext* bc::bcvm::importTest(string p_t)
{
	vm::bcExecContext* ec = nullptr;
	string script = importTestAsString(p_t);
	getData()->src.clear(); 
	getData()->src.push_back(script);
	
	//compile source to an execution context, check for errors
	comp::bcCompiler c;
	c.startup();
	return c.compile(&bc::bcvm::getData()->src);
}

string bc::bcvm::importTestAsString(string p_t)
{
	if (p_t == "1")
		return "int a;";
	else if (p_t == "2")
		return "int a = 2;";
	else if (p_t == "3")
		return
		"int a = 2; \n\
int b = 3;\n\
int c = a + b;";
	else if(p_t == "4")
		return
"int a =2; \n\
int b = 3;\n\
int c = a + b;\n\
func int main()\n\
{\n\
	int mainvar = a*c;\n\
	return mainvar;\n\
}";
	else
		return ";";

}

vector<string> bc::bcvm::loadFileAsStrings(const char* p_f)
{
	vector<string> out;

	bc::util::readFile(p_f, &out);

	return out;
}


void bc::bcvm::run()
{
	//run script until halt
	while (!data.vm.con->halt)
		data.vm.exec(1);
}

bool bc::bcvm::isRunning()
{
	return (!data.vm.con->halt);
}