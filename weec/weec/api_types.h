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
			wcScript(error::wcError);

			error::wcError getError();
			void setError(error::wcError);

			bytecode::wcExecContext con;
			error::wcError error;
			int handle;
		};

		struct wcContext
		{

		};
	}
}
#endif