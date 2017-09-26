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

			wcError getError();
			void setError(wcError);

			virtual api::wcScript compile(std::vector<std::string> input);

		protected:
			wcError error;
		};

		class wcClassicCompiler : public wcBaseCompiler
		{
		public:
			wcClassicCompiler();

			virtual api::wcScript compile(std::vector<std::string> input);
		};

		class wcHostedEXECompiler : public wcBaseCompiler
		{
		public:

		};
	}
}
#endif