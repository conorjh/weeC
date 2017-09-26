#ifndef WCC_IO_H
#define WCC_IO_H
#include "api.h"

namespace wcc
{
	namespace io
	{
		std::vector<std::string> importSource(std::string p_filename);

		bool exportAST(wc::parse::wcAST&, std::string outputFilename);
		bool exportBytecode(wc::bytecode::wcExecContext&, std::string outputFilename);
	}
}


#endif 