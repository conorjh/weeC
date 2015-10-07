#ifndef BCC_IMPORT_H
#define BCC_IMPORT_H

#include <string>
#include <vector>
#include "bcc.h"

namespace bc
{
	namespace bcc
	{
		namespace imp
		{
			bc::vm::bcExecContext* importScriptFromFile(std::string);
			bc::vm::bcExecContext* importByteCodeFromFile(std::string);
		}
	}
}

#endif
