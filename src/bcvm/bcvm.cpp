#include "bcvm.h"
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
	//compile the runFile
	if (getData()->runFileIsByteCode)
		data.vm.con = bc::imp::importByteCodeFromFile(data.runFile);
	else
		data.vm.con = bc::imp::importScriptFromFile(data.runFile);

	
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
	cin >> in;
	return in;
}

int bc::bcvm::consoleParseCmd(string p_in)
{
	vector<string> tokens = bc::lex::tokenize(p_in," ");
	if (tokens.size() == 0)
		return 0;
	if (tokens.at(0) == "runtest")
		if (tokens.size() > 1 && tokens.at(1) == "1")
		

		
		else if
		

		
	
	return 1;
}

int bc::bcvm::consoleLoop()
{
	//get input
	string in = consoleGetInput();
	
	//parse input
	consoleParseCmd(in);

	//do input
	

	return 1;
}

void bc::bcvm::run()
{
	//main loop
	while (!data.vm.con->halt)
		data.vm.exec(1);
}

bool bc::bcvm::isRunning()
{
	return (!data.vm.con->halt);
}