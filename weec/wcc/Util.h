#ifndef UTIL_H
#define UTIL_H
#include <vector>
#include <string>


namespace Error
{
	struct ErrorData
	{
		void AddError(std::string ErrorToAdd) { return Errors.push_back(ErrorToAdd); };
		bool HasErrored() const { return Errors.size(); }
		std::vector<std::string> Errors;
	};
}
	namespace Util
	{
		unsigned int RandomNumU(unsigned int Max);
		bool FileExists(std::string Filepath);
		std::string uitos(unsigned int i);
		std::string itos(int i);		
	}


#endif
