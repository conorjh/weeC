#include "wcvm.h"
#include <vector>

using namespace std;
using namespace wcvm;
using namespace wc;

void wcvm::exec()
{
}

void wcvm::init(int argc, char * argv[])
{

}

void wcvm::cleanup()
{
}

wcvm::wcvmData::wcvmData()
{

}
void wcvm::parseCmdLine(int p_argc, char *p_argv[])
{
	//convert to an easier to use format
	vector<CmdLineArg> args = convertCmdLineArgs(p_argc, p_argv);

	//process each command line argument
	for (int t = 0; t < args.size(); ++t)
		parseCmdLine_Arg(args[t]);

}

//adjust program settings according to command line parameters
bool wcvm::parseCmdLine_Arg(CmdLineArg p_arg)
{
	/*
	switch (p_arg.type)
	{
	case free_floating_parameter:
		p_arg.cmdlet = cmdlet_source;
	case command:
	case optional:
		if (p_arg.cmdlet == cmdlet_output)			//-o    Output filename
		{
			if (!p_arg.params.size())
				return false;
			data.filenameOutput = p_arg.params[0];
		}
		else if (p_arg.cmdlet == cmdlet_target)		//-t	Target Compiler
		{
			if (!p_arg.params.size())
				return false;	//we need at least 1 parameter
			if (p_arg.params[0] == "bytecode" || p_arg.params[0] == "bc")
				data.compileTarget = wcct_bytecode;
			else if (p_arg.params[0] == "simple_bytecode" || p_arg.params[0] == "simple")
				data.compileTarget = wcct_simple_bytecode;
			else if (p_arg.params[0] == "ansi_c" || p_arg.params[0] == "c")
				data.compileTarget = wcct_ansi_c;
			else
				data.compileTarget = wcct_x86;
		}
		else if (p_arg.cmdlet == cmdlet_source)		//-s      Source filename
		{
			if (!p_arg.params.size())
				return false;
			data.filenameSource = p_arg.params[0];
		}
		else if (p_arg.cmdlet == cmdlet_h)			//-h (hide display)
		{
			data.displayOutput = false;
		}
		else if (p_arg.cmdlet == cmdlet_console)	//-console   Console input active
		{
			data.consoleOn = true;
		}
		else if (p_arg.cmdlet == cmdlet_ast)		//-ast    AST Filename
		{
			data.outputAST = true;
			if (p_arg.params.size())
				data.filenameAST = p_arg.params[0];
		}
		return true;
	}
	return true;
	*/
}

vector<CmdLineArg> wcvm::convertCmdLineArgs(int p_argc, char *p_argv[])
{
	/*
	vector<CmdLineArg> argOutput;

	CmdLineArg arg;
	bool parsingParams = false;
	int index = 1;	//skip the 1st argument
	while (index < p_argc)
	{
		switch (p_argv[index][0])
		{
			//command
			//optional command
		case '\\':
		case '-':
		case '--':
			if (parsingParams)
			{
				argOutput.push_back(arg);
				arg = CmdLineArg();
			}

			if (p_argv[index][0] == '--')
				arg.type = optional;
			else
				arg.type = command;

			arg.cmdlet = string(p_argv[index]).substr(1, strlen(p_argv[index]) - 1);
			parsingParams = true;
			break;

			//parameter
		default:
			arg.params.push_back(p_argv[index]);
			if (!parsingParams)
			{
				arg.type = free_floating_parameter;
				arg.cmdlet = p_argv[index];
				argOutput.push_back(arg);
				arg = CmdLineArg();
			}
			break;
		}
		index++;
	}

	if (parsingParams)
		argOutput.push_back(arg);
	return argOutput;
	*/
}