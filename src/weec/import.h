#ifndef wc_IMPORT_H
#define wc_IMPORT_H

#include <string>
#include <vector>
#include "vm.h"

namespace wc
{
	struct wcScript;
	namespace imp
	{
		//import to wcExecContext (compiled bytecode)
		vm::wcExecContext* importScriptFromFile(std::string);
		vm::wcExecContext* importByteCodeFromFile(std::string);

		//import to a Script (compiled bytecode + debug info (source, symbols etc))
		wcScript importScriptFromFileToScript(std::string);
		wcScript importScriptFromVectorToScript(std::vector<std::string>);
		wcScript importByteCodeFromFileToScript(std::string);
		wcScript importScriptFromStringToScript(std::string);


	}
}


#endif
