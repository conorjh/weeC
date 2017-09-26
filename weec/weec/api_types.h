#ifndef WC_API_TYPES_H
#define WC_API_TYPES_H
#include "bytecode.h"
#include "error.h"

namespace wc
{
	namespace api
	{
		struct wcScript
		{
			wcScript();
			wcScript(wcError);

			wcError getError();
			void setError(wcError);

			bytecode::wcExecContext con;
			wcError error;
			int handle;
		};

		struct wcContext
		{

		};
	}
}
#endif