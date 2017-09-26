#ifndef WC_API_H
#define WC_API_H
#include <string>
#include "api_types.h"
#include "compiler.h"
#include "simple.h"
#include "weec.h"

namespace wc
{
	namespace api
	{
		
		//load/compile scripts
		wcScript loadScriptFromFile(std::string p_filename);

		//run scripts

	}
}
#endif