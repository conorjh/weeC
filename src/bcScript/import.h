#ifndef BC_IMPORT_H
#define BC_IMPORT_H

#include <string>
#include <vector>
#include "vm.h"

namespace bc
{
	struct bcScript;
	namespace imp
	{
		//import to bcExecContext (compiled bytecode)
		vm::bcExecContext* importScriptFromFile(std::string);
		vm::bcExecContext* importByteCodeFromFile(std::string);

		//import to a Script (compiled bytecode + debug info (source, symbols etc))
		bc::bcScript importScriptFromFileToScript(std::string);
		bc::bcScript importScriptFromVectorToScript(std::vector<std::string>);
		bc::bcScript importByteCodeFromFileToScript(std::string);
		bc::bcScript importScriptFromStringToScript(std::string);


	}
}


#endif
