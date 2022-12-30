#include "Wcc.h"

wcc::CompilerProfile::CompilerProfile()
{
	//defaults
	Action = CompilerAction::CompileAndRun;
	BinaryType = CompilerBinaryType::AST;
	VerboseOutput = false;
	BuildSymbolTable = false;
	SymbolTableFilepath = "";
	DestinationFilepath = SourceFilepath = "";
}
