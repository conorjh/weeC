#include "Wcc.h"

wcc::CompilerProfile::CompilerProfile()
{
	//defaults
	Action = CompilerAction::REPL;
	BinaryType = CompilerBinaryType::AST;
	VerboseOutput = false;
	BuildSymbolTable = false;
	SymbolTableFilepath = "";
	DestinationFilepath = SourceFilepath = "";
}
