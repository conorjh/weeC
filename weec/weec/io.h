#ifndef WC_IO_H
#define WC_IO_H
#include "api.h"
#include "parse.h"

namespace wc
{
	namespace io
	{
		std::vector<std::string> importSource(std::string p_filename);
		bool exportSource(std::string filename, std::string content);

		bool exportAST(wc::parse::wcAST&, std::string outputFilename);
		bool importAST(wc::parse::wcAST&, std::string inputFilename);
		bool exportBytecode(wc::bytecode::wcExecContext&, std::string outputFilename);
		bool importBytecode(wc::bytecode::wcExecContext&, std::string inputFilename);
	}
}


#endif
