#ifndef BCC_EXPORT_H
#define BCC_EXPORT_H

#include <string>
#include <vector>
#include "bcscript.h"

namespace bc
{
	namespace bcc
	{
		namespace exp
		{
			void exportToByteCodeFile(vm::bcExecContext*, std::string);
		}
	}
}

#endif
