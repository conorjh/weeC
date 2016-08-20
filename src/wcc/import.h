#ifndef WEEC_IMPORT_H
#define WEEC_IMPORT_H

#include <string>
#include <vector>
#include "wcc.h"

namespace wc
{
	namespace wcc
	{
		namespace imp
		{
			wc::vm::wcExecContext* importScriptFromFile(std::string);
			wc::vm::wcExecContext* importByteCodeFromFile(std::string);
		}
	}
}

#endif
