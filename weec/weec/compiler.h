#ifndef WC_COMPILER_H
#define WC_COMPILER_H
#include "api_types.h"
#include "codegen.h"

namespace wc
{
	namespace compile
	{
		class wcBaseCompiler
		{
		public:
			wcBaseCompiler();

			error::wcError getError();
			void setError(error::wcError);

			virtual api::wcScript compile(std::vector<std::string> input);

		protected:
			error::wcError error;
		};

		class wcHostedEXECompiler : public wcBaseCompiler
		{
		public:

		};
	}
}
#endif