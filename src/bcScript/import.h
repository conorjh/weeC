#ifndef BC_IMPORT_H
#define BC_IMPORT_H

#include <string>
#include <vector>
#include "vm.h"

namespace bc
{
	namespace imp
	{
		vm::bcExecContext* importScriptFromFile(std::string);
		vm::bcExecContext* importByteCodeFromFile(std::string);
	}
}


#endif
