#ifndef WEEC_EXPORT_H
#define WEEC_EXPORT_H

#include <string>
#include <vector>
#include "vm.h"

namespace wc
{
		namespace exp
		{
			void exportToByteCodeFile(vm::wcExecContext*, std::string);
		}
}

#endif
