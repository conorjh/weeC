#include "wcc.h"
#include "..\weec\import.h"
#include "..\weec\export.h"

using namespace std;
using namespace wc::wcc;

namespace wc
{
	namespace wcc
	{
		wccData data;
	}
}

void wc::wcc::startup()
{
	data.compiler.startup();
	data.exportDelimiter = ",";
}

void wc::wcc::shutdown()
{

}

wccData* wc::wcc::getData()
{
	return &data;
}

vector<string>* wc::wcc::loadFileAsStrings(const char* p_f)
{
	vector<string>* out=new vector<string>;

	if (!wc::util::readFile(p_f, out))
	{
		delete out;
		return nullptr;
	}
	
	return out;
}

int wc::wcc::compile()
{
	//check we have a source file
	if (data.origSource == "")
		//no source
		return -1;

	//attempt to load it
	vector<string>* source = loadFileAsStrings(data.origSource.c_str());
	if (source == nullptr)
	{
		//error loading source 
		return -1;
	}
	
	//compile source to an execution context, check for errors
	comp::wcCompiler c;
	vm::wcExecContext* ec = c.compile(source);
	if (ec == nullptr)
	{
		//error
		return -1;
	}

	//make a destination filename if we arent given one
	if (data.compileTarget == "")
	{
		data.compileTarget = data.origSource + ".wcs";
		wc::exp::exportToByteCodeFile(ec, data.compileTarget);
	}

	//report success to console


	return 0;
}

//returns the new argument index
int wc::wcc::parseCmdLineArg(const char * p_args[], int p_i)
{
	int index = p_i;
	string arg(p_args[index]);

	//string literal, must be preceded by a command (eg   -c "filename.wcs")
	if (arg[0] == '"' && !index)
	{
		//opening string literal, trick parseCmdLineArg_Compile into thinking
		//we've already had a -c or -compile
		--index;
		return index = parseCmdLineArg_Compile(p_args, index);
	}
	//compile the given source code
	if ((arg == "-c" || arg == "-compile") && data.origSource == "")
	{
		//specify filename, next token should be a string literal
		return index = parseCmdLineArg_Compile(p_args, index);
	}
	else if (data.origSource != "" && (arg == "-c" || arg == "-compile"))
	{
		//error - we opened the cmdline with a string literal, but now want to compile again 
		return index;
	}
	else if (arg == "-t" || arg == "-target")
	{
		//specify filename, next token should be a string literal
		return index = parseCmdLineArg_Target(p_args, index);
	}
	return index;
}

//parse the -c or -compile command
int wc::wcc::parseCmdLineArg_Compile(const char * p_args[], int p_i)
{
	//consume the -c or -compile
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
		//no closing quote
		return p_i;
	}
	else
	{
		//valid string format
		data.origSource = arg;
	}

	//return index;
	return p_i;

}

//parse the -t or -target command
int wc::wcc::parseCmdLineArg_Target(const char * p_args[], int p_i)
{
	//consume the -t or -target
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
		data.compileTarget = arg;

	//return index;
	return p_i;

}

void wc::wcc::exportToByteCodeFile(vm::wcExecContext *, std::string)
{

}
