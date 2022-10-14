#include "wcc.h"
#include "api.h"
#include "util.h"
#include "classic.h"
#include "io.h"
#include <string>
#include <iostream>

using namespace std;
using namespace wcc;
using namespace wc;
using namespace wc::compile;
using namespace wc::util;
using namespace wc::bytecode;
using namespace wc::error;
using namespace wc::api;
using namespace wc::parse;
using namespace wc::io;

namespace wcc
{
	wccData data;

	//methods
	CompilerOutput compile();
	void compile_exportOutput(CompilerOutput&);
	void splash();
	void print();
	void print(string);
	void displayPreCompileInfo();
	void displayCompileResult(CompilerOutput);
	bool parseCmdLine_Arg(CmdLineArg p_arg);

	//helpers
	wc::compile::wcBaseCompiler getTargetCompiler(CompileTarget);
	SourceType deriveInputType(std::string p_filename);
	std::string getSourceLine(int line);
	std::string getFilenameWithoutExt(string);
	std::string getExtensionFromTargetPlatform(CompileTarget);
	std::string getFilenameExtension(string);

	//consts
	const char * cmdlet_output = "o";			//output filename
	const char * cmdlet_source = "s";			//source filename
	const char * cmdlet_target = "t";			//compilation target
	const char * cmdlet_console = "console";	//console active
	const char * cmdlet_h = "h";				//hide output
	const char * cmdlet_ast = "ast";			//export AST (filename optional, uses output filename)

	const char * fext_sourcecode = ".wc";
	const char * fext_bytecode = ".wcc";
	const char * fext_ansi_c = ".c";
	const char * fext_wasm = ".wasm";
	const char * fext_ast = ".ast";
	const char * fext_x86 = ".exe";
}

wcc::wccData::wccData()
{
	sourceType = wcst_sourcecode;
	compileTarget = wcct_bytecode;
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


void wcc::parseCmdLine(int p_argc, char *p_argv[])
{
	//convert to an easier to use format
	vector<CmdLineArg> args = convertCmdLineArgs(p_argc, p_argv);

	//process each command line argument
	for (int t = 0; t < args.size(); ++t)
		parseCmdLine_Arg(args[t]);

	//output filename required even if not specified by user
	if (data.filenameOutput == "")
		data.filenameOutput = getFilenameWithoutExt(data.filenameSource) + getExtensionFromTargetPlatform(data.compileTarget);

	//use the output filename for the AST if one wasnt specified
	if (data.outputAST && data.filenameAST == "")
		data.filenameAST = getFilenameWithoutExt(data.filenameOutput) + fext_ast;
}

//adjust program settings according to command line parameters
bool wcc::parseCmdLine_Arg(CmdLineArg p_arg)
{
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
		else
			return false;
		return true;
	}
	return true;
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

	if (parsingParams)
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
	switch (data.compileTarget)
	{
	case wcct_bytecode:
		output.compiler = classicComp = static_cast<wcClassicCompiler*>(&getTargetCompiler(data.compileTarget));
		output.script = classicComp->wcClassicCompiler::compile(importSource(data.filenameSource), &output.ast);
		break;
	case wcct_simple_bytecode:
		output.compiler = simpleComp = static_cast<wcSimpleCompiler*>(&getTargetCompiler(data.compileTarget));
		output.script = simpleComp->wcSimpleCompiler::compile(importSource(data.filenameSource), &output.ast);
		break;
	default:	case wcct_x86:	case wcct_ansi_c:
		print("Currently unsupported Target Platform (" + targetStrings.find(data.compileTarget)->second + ")");
		return output;
	}

	//export our compiled data
	compile_exportOutput(output);

	//display result/error etc
	displayCompileResult(output);

	return output;
}

void wcc::compile_exportOutput(CompilerOutput& output)
{
	//export the AST if required
	if (data.outputAST)
		wc::io::exportAST(output.ast, data.filenameAST);

	//export the compilation in appropriate format
	switch (data.compileTarget)
	{
	case wcct_bytecode:
	case wcct_simple_bytecode:
		exportBytecode(output.script.con, data.filenameOutput);
		break;

	case wcct_ansi_c:
	case wcct_x86:
	default:
		//currently unimplemented
		break;
	}
}

void wcc::print()
{
	print("");
}

void wcc::print(string input)
{
	if (!data.displayOutput)
		return;

	cout << input;
	if (input.size() && input[input.length() - 1] != '\n')
		cout << "\n";
	else if (!input.size())
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
	else if (ext == fext_wasm)
		return wcst_wasm;
	else if (ext == fext_ast)
		return wcst_ast;
	else
		return wcst_sourcecode;
}

string wcc::getSourceLine(int p_line)
{
	return importSource(data.filenameSource)[p_line];
}

string wcc::getFilenameWithoutExt(string input)
{
	if (input.size() < 3)
		return input;

	//work backwards to find the final period
	int index = input.size() - 1;
	while (index != 0)
		if (input[index] == '.')
			break;
		else
			index--;

	//no . found, return whole input
	if (index == input.size())
		return input;

	return input.substr(0, input.size() - (input.size() - index));
}

//takes a CompileTarget, returns the string rep of it
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

string wcc::getFilenameExtension(string input)
{
	int index = input.size();
	while (index > 0 && input[index] != '.')
		index--;
	return input.substr(index);
}

//display compile settings
void wcc::displayPreCompileInfo()
{
	if (data.filenameSource == "")
	{
		print("No source file specified");
		return;	//no source file specified
	}

	print("Filename: " + data.filenameSource + "\t("
		+ sourceTypeStrings.find(deriveInputType(data.filenameSource))->second + ")");
	print("Target Platform: " + targetStrings.find(data.compileTarget)->second);

	if (data.filenameAST != "")
		print("\t-AST: " + data.filenameAST);

	print("");
	print("Compiling " + data.filenameOutput + "...");
}

//
void wcc::displayCompileResult(CompilerOutput input)
{
	wcError err = input.compiler->getError();
	if (err.code)
	{
		print("Compilation failed...");
		print("- - - - - - - - - - - -");
		print(string((int)err.line + 1) + "," + string((int)err.col) + ": \"" + getSourceLine(err.line) + "\"");
		print("Error " + string((int)err.code) + " " + errorStrings.find((int)err.code)->second + ":  \"" + err.text + "\"");
		print("- - - - - - - - - - - -");
	}
	else
	{
		print("Compilation complete");
	}
}

