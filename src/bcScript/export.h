#ifndef BC_EXPORT_H
#define BC_EXPORT_H

#include <string>
#include <vector>
#include "vm.h"

namespace bc
{
		namespace exp
		{
			void exportToByteCodeFile(vm::bcExecContext*, std::string);
		}
}

#endif
