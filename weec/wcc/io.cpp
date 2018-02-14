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
		string exportASTToText_writeChild(tree<wcParseNode>::pre_order_iterator& p_index);
		string exportASTToText_writeSibling(tree<wcParseNode>::pre_order_iterator& p_index);
		string exportASTToText_writeParent(tree<wcParseNode>::pre_order_iterator& p_index);
	}
}

bool wcc::io::exportAST(wc::parse::wcAST &p_ast, std::string p_outputFilename)
{
	tree<wcParseNode>::pre_order_iterator index = p_ast.parseTree.begin();
	++index;
	int lastDepth = 0;
	string outputString = "";
	while (index != p_ast.parseTree.end())
	{
		if (p_ast.parseTree.depth(index) > lastDepth)
		{
			//we are writing children of the previous node
			outputString += exportASTToText_writeChild(index);
		}
		else if (p_ast.parseTree.depth(index) < lastDepth)
		{
			//we are back to the parent node
			outputString += exportASTToText_writeChild(index);
		}
		else if(p_ast.parseTree.depth(index) == lastDepth)
		{
			//we are writing a sibling
			outputString += exportASTToText_writeSibling(index);
		}
		else if (lastDepth == -1)
		{
			//we are writing the initial head of the tree

		}

		lastDepth = p_ast.parseTree.depth(index);
	}

	return false;
}

string wcc::io::exportASTToText_writeChild(tree<wcParseNode>::pre_order_iterator& p_index)
{
	return "+child=" + p_index.node->data.type;
}

string wcc::io::exportASTToText_writeSibling(tree<wcParseNode>::pre_order_iterator& p_index)
{
	return "+sibling=" + p_index.node->data.type;
}

string wcc::io::exportASTToText_writeParent(tree<wcParseNode>::pre_order_iterator& p_index)
{
	return "+backtoparent" + p_index.node->data.type;
}

bool wcc::io::exportBytecode(wc::bytecode::wcExecContext &p_context, std::string p_outputFilename)
{
	return false;
}

