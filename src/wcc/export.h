#ifndef WEEC_EXPORT_H
#define WEEC_EXPORT_H

#include <string>
#include <vector>
#include "..\weec\weec.h"

namespace wc
{
	namespace wcc
	{
		namespace exp
		{
			void exportToByteCodeFile(vm::wcExecContext*, std::string);
		}
	}
}

#endif
