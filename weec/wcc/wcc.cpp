#include "wcc.h"
#include "api.h"
#include "util.h"
#include "classic.h"
#include <iostream>

using namespace std;
using namespace wcc;
using namespace wcc::io;
using namespace wc;
using namespace wc::compile;
using namespace wc::util;
using namespace wc::bytecode;
using namespace wc::error;
using namespace wc::api;

namespace wcc
{
	wccData data;

	//methods
	CompilerOutput compile();
	void splash();
	void print();
	void print(string);
	void displayPreCompileInfo();
	void displayCompileResult(CompilerOutput);
	bool parseCmdLine_Arg(CmdLineArg p_arg);
	
	//helpers
	vector<CmdLineArg> convertCmdLineArgs(int, char *[]);
	wcBaseCompiler getTargetCompiler(CompileTarget);
	SourceType deriveInputType(string p_filename);
	string getSourceLine(int line);
	string getFilenameWithoutExt(string);
	string getExtensionFromTargetPlatform(CompileTarget);
	string getFilenameExtension(string);

	//consts
	const char * cmdlet_output = "o";
	const char * cmdlet_source = "s";
	const char * cmdlet_target = "t";
	const char * cmdlet_console = "console";
	const char * cmdlet_h = "h";
	const char * cmdlet_ast = "ast";

	const char * fext_sourcecode = ".wc";
	const char * fext_bytecode = ".wcc";
	const char * fext_ansi_c = ".c";
	const char * fext_x86 = ".exe";
}

wcc::wccData::wccData()
{
	sourceType = wcst_sourcecode;
	target = wcct_bytecode;
	filenameOutput = filenameSource = filenameAST = "";
	outputAST = consoleOn = false;
	displayOutput = true;
}

wcc::CmdLineArg::CmdLineArg()
{
	this->type = null;
}

wcBaseCompiler wcc::getTargetCompiler(CompileTarget p_target)
{
	switch (p_target)
	{
	case wcct_bytecode:
		return wcClassicCompiler();
	case wcct_simple_bytecode:
		return wcSimpleCompiler();
	case wcct_x86:
		return wcHostedEXECompiler();
	default:
		return wcClassicCompiler();
	}
}

vector<CmdLineArg> wcc::convertCmdLineArgs(int p_argc, char *p_argv[])
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
	
	if(parsingParams)
		argOutput.push_back(arg);
	return argOutput;
}

void wcc::exec()
{
	if (data.filenameSource == "")
		return;

	//attempt to compile
	CompilerOutput output = compile();
	
	//export compiled code
	
}

CompilerOutput wcc::compile()
{
	//print info
	displayPreCompileInfo();

	//compile with the appropriate compiler
	CompilerOutput output;	wcClassicCompiler* classicComp;	wcSimpleCompiler* simpleComp;
	switch (data.target)
	{
	case wcct_bytecode:
		output.compiler = classicComp = static_cast<wcClassicCompiler*>(&getTargetCompiler(data.target));
		output.script = classicComp->wcClassicCompiler::compile(importSource(data.filenameSource));
		break;
	case wcct_simple_bytecode:
		output.compiler = simpleComp = static_cast<wcSimpleCompiler*>(&getTargetCompiler(data.target));
		output.script = simpleComp->wcSimpleCompiler::compile(importSource(data.filenameSource));
		break;
	default:	case wcct_x86:	case wcct_ansi_c:
		print("Currently unsupported Target Platform ("+targetStrings.find(data.target)->second+")");
		return output;
	}
	
	//display result/error etc
	displayCompileResult(output);

	return output;
}

void wcc::print()
{
	print("");
}

void wcc::print(string p_input)
{
	if (!data.displayOutput)
		return;

	cout << p_input;
	if (p_input.size() && p_input[p_input.length() - 1] != '\n')
		cout << "\n";
	else if(!p_input.size())
		cout << "\n";
}

void wcc::splash()
{
	print("WeeC Compiler");
	print("- - - - - - - - - - - -");
}

void wcc::init(int argc, char *argv[])
{
	parseCmdLine(argc, argv);

	//output filename required even if not specified by user
	if (data.filenameOutput == "")
		data.filenameOutput = getFilenameWithoutExt(data.filenameSource) + getExtensionFromTargetPlatform(data.target);

	splash();
}

void wcc::cleanup()
{

}

SourceType wcc::deriveInputType(string p_filename)
{
	string ext = getFilenameExtension(p_filename);
	if (ext == fext_sourcecode)
		return wcst_sourcecode;
	else if (ext == fext_bytecode)
		return wcst_bytecode;
	else
		return wcst_sourcecode;
}

string wcc::getSourceLine(int p_line)
{
	return importSource(data.filenameSource)[p_line];
}

string wcc::getFilenameWithoutExt(string p_input)
{
	if (p_input.size() < 3)
		return p_input;

	int index = 0;
	while (index < p_input.size() && p_input[index] != '.')
		index++;
	return p_input.substr(0, p_input.size() - 3);
}

string wcc::getExtensionFromTargetPlatform(CompileTarget p_target)
{
	switch (p_target)
	{
	default:
	case wcct_bytecode:
	case wcct_simple_bytecode:
		return fext_bytecode;
	case wcct_x86:
		return fext_x86;
	case wcct_ansi_c:
		return fext_ansi_c;
	}
}

string wcc::getFilenameExtension(string p_input)
{
	int index = p_input.size();
	while (index > 0 && p_input[index] != '.')
		index--;
	return p_input.substr(index);
}

void wcc::displayPreCompileInfo()
{
	if (data.filenameSource == "")
	{
		print("No source file specified");
		return;	//no source file specified
	}

	print("Filename: " + data.filenameSource+ "\t(" 
		+ sourceTypeStrings.find(deriveInputType(data.filenameSource))->second +")");
	print("Target Platform: " + targetStrings.find(data.target)->second);
	
	if (data.filenameAST != "")
		print("\t-AST: " + data.filenameAST);

	print("");
	print("Compiling " + data.filenameOutput + "...");
}

void wcc::displayCompileResult(CompilerOutput p_input)
{
	wcError err = p_input.compiler->getError();
	if (err.code)
	{
		print("Compilation failed...");
		print("- - - - - - - - - - - -");
		print(itos((int)err.line + 1) + "," + itos((int)err.col) + ": \""+getSourceLine(err.line )+"\"");
		print("Error "+itos((int)err.code) + " " + errorStrings.find((int)err.code)->second + ":  \""+err.text+"\"" );
		print("- - - - - - - - - - - -");
	}
	else
	{
		print("Compilation complete");
	}
}

void wcc::parseCmdLine(int p_argc, char *p_argv[])
{
	//convert to an easier to use format
	vector<CmdLineArg> args = convertCmdLineArgs(p_argc, p_argv);

	//process each command line argument
	for (int t = 0; t < args.size(); ++t)
		parseCmdLine_Arg(args[t]);

}

bool wcc::parseCmdLine_Arg(CmdLineArg p_arg)
{
	switch (p_arg.type)
	{
	case free_floating_parameter:
		p_arg.cmdlet = cmdlet_source;
	case command:
	case optional:
		if (p_arg.cmdlet == cmdlet_output)			//-o
		{
			if (!p_arg.params.size())
				return false;
			data.filenameOutput = p_arg.params[0];
		}
		else if (p_arg.cmdlet == cmdlet_target)		//-t
		{
			if (!p_arg.params.size())
				return false;
			if (p_arg.params[0] == "bytecode" || p_arg.params[0] == "bc")
				data.target = wcct_bytecode;
			else if (p_arg.params[0] == "simple_bytecode" || p_arg.params[0] == "simple")
				data.target = wcct_simple_bytecode;
			else if (p_arg.params[0] == "ansi_c" || p_arg.params[0] == "c")
				data.target = wcct_ansi_c;
			else
				data.target = wcct_x86;
		}
		else if (p_arg.cmdlet == cmdlet_source)		//-s
		{
			if (!p_arg.params.size())
				return false;
			data.filenameSource = p_arg.params[0];
		}
		else if (p_arg.cmdlet == cmdlet_h)			//-h (hide display)
		{
			data.displayOutput = false;
		}
		else if (p_arg.cmdlet == cmdlet_console)	//-console
		{
			data.consoleOn = true;
		}
		else if (p_arg.cmdlet == cmdlet_ast)		//-ast
		{
			data.outputAST = true;
			if (!p_arg.params.size())
				return true;
			data.filenameAST = p_arg.params[0];
		}
		return true;
	}
	return true;
}