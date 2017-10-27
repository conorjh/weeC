#ifndef WC_UTIL_H
#define WC_UTIL_H
#include <vector>
#include <string>

namespace wc
{
	namespace util
	{
		 std::vector<std::string> loadFileAsText(std::string filename);
		 bool fileExists(std::string filename);
		 int randomInt();
		 unsigned int randomUInt();
		 std::string itos(int);
	}
}


#endif