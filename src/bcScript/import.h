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
		vm::bcExecContext* importScriptFromFile(std::string);
		vm::bcExecContext* importByteCodeFromFile(std::string);
		bc::bcScript importScriptFromFileToScript(std::string);
		bc::bcScript importByteCodeFromFileToScript(std::string);
	}
}


#endif
