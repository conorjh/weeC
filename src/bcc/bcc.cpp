#include "bcc.h"

namespace bc
{
	namespace bcc
	{

		bccData data;

	}
}

using namespace std;
using namespace bc::bcc;

void bc::bcc::startup()
{
	data.exportDelimiter = ",";
}

bccData* bc::bcc::getData()
{
	return &data;
}

vector<string>* bc::bcc::loadSourceCode(const char* p_f)
{
	vector<string>* out=new vector<string>;

	if (!bc::util::bcreadfile(p_f, out))
		return nullptr;
	
	return out;
}

int bc::bcc::compile()
{
	//check we have a source file
	if (data.destSource == "")
		//no source
		return -1;

	//attempt to load it
	vector<string>* source = loadSourceCode(data.destSource.c_str());
	if (source == nullptr)
	{
		//error loading source 
		return -1;
	}
	
	//compile source to an execution context, check for errors
	comp::bcCompiler c;
	vm::bcExecContext* ec = c.compile(source);
	if (ec == nullptr)
	{
		//error
		return -1;
	}

	
	//make a default destination filename if we arent given one
	if (data.destPCS == "")
	{
		bc::bcc::data.destPCS = data.destSource + ".bcs";
		exp::exportToByteCodeFile(ec, data.destPCS);
	}

	//report success to console

	return 0;
}

//returns the new index
void bc::bcc::parseCmdLineArg(const char * p_args[], int p_i)
{
	int index = p_i;
	string arg(p_args[index]);

	//string literal, must be preceded by a command (eg   -c "filename.bcs")
	if (arg[0] == '"' && !index)
	{
		//opening string literal, trick parseCmdLineArg_Compile into thinking
		//we've already had a -c or -compile
		--index;
		index = parseCmdLineArg_Compile(p_args, index);
	}
	//compile the given source code
	if ((arg == "-c" || arg == "-compile") && data.destSource == "")
	{
		//specify filename, next token should be a string literal
		index = parseCmdLineArg_Compile(p_args, index);
	}
	else if (data.destSource != "" && (arg == "-c" || arg == "-compile"))
	{
		//error - we opened the cmdline with a string literal, but now want to compile again 
		return;
	}
}

//parse the -c or -compile command
int bc::bcc::parseCmdLineArg_Compile(const char * p_args[], int p_i)
{
	//consume the -c or -compile
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
		data.destSource = arg;

	//return index;
	return p_i;

}
