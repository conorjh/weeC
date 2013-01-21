#pragma once
#include "types.h"
#include "error.h"
#include <vector>
#include <map>

namespace bc
{
	namespace parser
	{
		class bcParser
		{
		public:
			bcParser();

			int Parse();
		
		private:
			vector<bcToken>* in;
		};
	}
}