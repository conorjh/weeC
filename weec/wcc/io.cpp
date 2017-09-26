#include "io.h"
#include "util.h"
#include "tinyxml2\tinyxml2.h"
#include <vector>

using namespace std;
using namespace wc;
using namespace wc::parse;
using namespace wc::util;
using namespace tinyxml2;

vector<string> wcc::io::importSource(string filename)
{
	return wc::util::loadFileAsText(filename);
}

namespace wcc
{
	namespace io
	{
		void exportAST_writeChildren(wc::parse::wcAST &p_ast, tree<wcParseNode>::pre_order_iterator& p_index, string& outputString);
		void exportAST_writeSibling(wc::parse::wcAST &p_ast, tree<wcParseNode>::pre_order_iterator& p_index, string& outputString);
		void exportAST_writeParent(wc::parse::wcAST &p_ast, tree<wcParseNode>::pre_order_iterator& p_index, string& outputString);
	}
}

bool wcc::io::exportAST(wc::parse::wcAST &p_ast, std::string p_outputFilename)
{
	XMLDocument output;

	tree<wcParseNode>::pre_order_iterator index = p_ast.parseTree.begin();
	XMLNode * pRoot = output.NewElement(pnTypeStrings.find(index->type)->second);
	++index;
	while (index != p_ast.parseTree.end())
	{

	}


	return false;
}

void wcc::io::exportAST_writeChildren(wc::parse::wcAST &p_ast, tree<wcParseNode>::pre_order_iterator& p_index, string& outputString)
{
	auto end = p_ast.parseTree.end();
	auto oldIndex = p_index;
	while (p_index != p_ast.parseTree.end())
	{
		//write our current node to output
		outputString += "child;" + itos(((int)p_index->type)) + ";";

		//go to next node
		p_index++;
		if (p_index == end)
			return;

		//decide what to do
		if (p_ast.parseTree.depth(oldIndex) < p_ast.parseTree.depth(p_index))
		{
			exportAST_writeChildren(p_ast, p_index, outputString);
		}
		else if(p_ast.parseTree.depth(oldIndex) == p_ast.parseTree.depth(p_index))
		{
			//no more children to write

			return;
		}
		else if (p_ast.parseTree.depth(oldIndex) > p_ast.parseTree.depth(p_index))
		{
			//no mo kids
			exportAST_writeParent(p_ast, p_index, outputString);
		}
	}
}

void wcc::io::exportAST_writeSibling(wc::parse::wcAST &p_ast, tree<wcParseNode>::pre_order_iterator& p_index, string& outputString)
{

}

void wcc::io::exportAST_writeParent(wc::parse::wcAST &p_ast, tree<wcParseNode>::pre_order_iterator& p_index, string& outputString)
{

}

bool wcc::io::exportBytecode(wc::bytecode::wcExecContext &p_context, std::string p_outputFilename)
{
	return false;
}

