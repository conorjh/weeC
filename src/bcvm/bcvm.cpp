#include "bcvm.h"
#include <vector>
#include <string>

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

//returns the new index
void bc::bcvm::parseCmdLineArg(const char * p_args[], int p_i)
{
	int index = p_i;
	string arg(p_args[index]);

	//string literal, must be preceded by a command (eg   -c "filename.bcs")
	if (arg[0] == '"' && !index)
	{
		//opening string literal, trick parseCmdLineArg_Run into thinking
		//we've already had a -r or -run
		--index;
		index = parseCmdLineArg_Run(p_args, index);
	}
	//compile the given source code
	if ((arg == "-r" || arg == "-run") && data.runFile == "")
	{
		//specify filename, next token should be a string literal
		index = parseCmdLineArg_Run(p_args, index);
	}
	else if (data.runFile != "" && (arg == "-r" || arg == "-run"))
	{
		//error - we opened the cmdline with a string literal, but now want to issue anothe run command
		return;
	}
}

//parse the -r or -run command
int bc::bcvm::parseCmdLineArg_Run(const char * p_args[], int p_i)
{
	//consume the -r or -run
	p_i++;
	string arg(p_args[p_i]);

	//expect only a file name in quotes
	if (arg[0] != '"')
		//error
		return p_i;
	else if (arg[arg.size() - 1] != '"')
		//no closing quote
		return p_i;
	else
		//valid string format
		data.runFile = arg;

	//return index;
	return p_i;

}

bcvmData* bc::bcvm::getData()
{
	return &bc::bcvm::data;
}

void bc::bcvm::run()
{
	//main loop
	
}