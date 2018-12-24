#include "wcvm.h"
#include <vector>
#include <iostream>
using namespace std;
using namespace wcvm;
using namespace wc;
using namespace wc::bytecode;
using namespace wc::vm;
using namespace wc::io;
namespace wcvm
{
	wcvmData data;
	void splash();
	void print();
	void print(string);

	//consts
	const char * cmdlet_source = "s";			//source filename
	const char * cmdlet_sourcetype = "t";			//source type (bytecode,simple bytecode
	const char * cmdlet_hideconsole = "hc";		//hide the input console when running
	const char * cmdlet_hideoutput = "h";		//hide any output

	const char * fext_bytecode = ".wcc";
}

wcvm::CmdLineArg::CmdLineArg()
{
	type = null;
}

void wcvm::print()
{
	print("");
}

void wcvm::print(string input)
{
	if (data.hideOutput)
		return;

	cout << input;
	if (input.size() && input[input.length() - 1] != '\n')
		cout << "\n";
	else if (!input.size())
		cout << "\n";
}

void wcvm::splash()
{
	print("WeeC VM");
	print("- - - - - - - - - - - -");
}

void wcvm::exec()
{
	wcExecContext con;	wcSimpleExecContext scon;
	wcClassicVM cvm;	wcSimpleVM svm;
	int handle;
	//import source
	if (data.sourceType == wcvmst_bytecode)
	{
		importBytecode(con, data.filenameSource);
		handle = cvm.load(con);
	}
	else
	{
		importBytecode(scon, data.filenameSource);
		handle = svm.load(scon);
	}

	//execute
	if (data.sourceType == wcvmst_bytecode)
		cvm.exec(handle);
	else
		svm.exec(handle);

}

void wcvm::init(int argc, char * argv[])
{
	parseCmdLine(argc, argv);

	splash();
}

void wcvm::cleanup()
{

}

wcvm::wcvmData::wcvmData()
{
	filenameSource = "";
	hideConsole = false;
	sourceType = wcvmst_bytecode;
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
	switch (p_arg.type)
	{
	case free_floating_parameter:
		p_arg.cmdlet = cmdlet_source;
	case command:
	case optional:
		if (p_arg.cmdlet == cmdlet_sourcetype)				//-t	source type
		{
			if (!p_arg.params.size())
				return false;	//we need at least 1 parameter
			if (p_arg.params[0] == "bytecode" || p_arg.params[0] == "bc")
				data.sourceType = wcvmst_bytecode;
			else if (p_arg.params[0] == "simple_bytecode" || p_arg.params[0] == "simple")
				data.sourceType = wcvmst_simple_bytecode;
		}
		else if (p_arg.cmdlet == cmdlet_source)			//-s      Source filename
		{
			if (!p_arg.params.size())
				return false;//we need at least 1 parameter
			data.filenameSource = p_arg.params[0];
		}
		else if (p_arg.cmdlet == cmdlet_hideoutput)			//-h      Hide any output
		{
			data.hideOutput = true;
		}
		else if (p_arg.cmdlet == cmdlet_hideconsole)	//-hc    hides the console
		{
			data.hideConsole = true;
		}
	}
	return true;
	
}

vector<CmdLineArg> wcvm::convertCmdLineArgs(int p_argc, char *p_argv[])
{
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
}