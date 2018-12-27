#include "io.h"
#include "util.h"
#include "bytecode.h"
//#include "tinyxml2\tinyxml2.h"
#include <vector>
#include <fstream>

using namespace std;
using namespace wc;
using namespace wc::parse;
using namespace wc::bytecode;
using namespace wc::util;
//using namespace tinyxml2;

namespace wc
{
	namespace io
	{
		string exportASTToText_writeChild(tree<wcParseNode>::pre_order_iterator& p_index);
		string exportASTToText_writeSibling(tree<wcParseNode>::pre_order_iterator& p_index);
		string exportASTToText_writeParent(tree<wcParseNode>::pre_order_iterator& p_index);
	}
}

vector<string> wc::io::importSource(string filename)
{
	return wc::util::loadFileAsText(filename);
}

bool wc::io::exportSource(string filename, string content)
{
	std::ofstream out(filename);
	out << content;
	out.close();
	return true;
}

bool wc::io::exportAST(wc::parse::wcAST &p_ast, std::string p_outputFilename)
{
	tree<wcParseNode>::pre_order_iterator index = p_ast.parseTree.begin();
	int lastDepth = -1;
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
		else if (p_ast.parseTree.depth(index) == lastDepth)
		{
			//we are writing a sibling
			outputString += exportASTToText_writeSibling(index);
		}
		else if (lastDepth == -1)
		{
			//we are writing the initial head of the tree
			outputString += "#head" + '\n';
		}
		lastDepth = p_ast.parseTree.depth(index);
		++index;
	}

	//write to file
	return exportSource(p_outputFilename, outputString);
}

string wc::io::exportASTToText_writeChild(tree<wcParseNode>::pre_order_iterator& p_index)
{
	//wcParseNode type
	string output = "#child type=" + itos(p_index.node->data.type) + "\n";

	//add all tokens
	for (int t = 0; t < p_index.node->data.tokens.size(); ++t)
		output += "#token type=" + itos(p_index.node->data.tokens[t].type) + " line=" + itos(p_index.node->data.tokens[t].line)
		+ " col=" + itos(p_index.node->data.tokens[t].column) + " data=" + p_index.node->data.tokens[t].data + "\n";

	return output;
}

string wc::io::exportASTToText_writeSibling(tree<wcParseNode>::pre_order_iterator& p_index)
{
	//wcParseNode type
	string output = "#node type=" + itos(p_index.node->data.type) + "\n";

	//add all tokens
	for (int t = 0; t < p_index.node->data.tokens.size(); ++t)
		output += "#token type=" + itos(p_index.node->data.tokens[t].type) + " line=" + itos(p_index.node->data.tokens[t].line)
		+ " col=" + itos(p_index.node->data.tokens[t].column) + " data=" + p_index.node->data.tokens[t].data + "\n";

	return output + "\n";
}

string wc::io::exportASTToText_writeParent(tree<wcParseNode>::pre_order_iterator& p_index)
{
	return "#backtoparent\n";
}

bool wc::io::exportBytecode(wc::bytecode::wcExecContext &p_context, std::string p_outputFilename)
{
	string output;
	for (int t = 0; t < p_context.instructions.size(); ++t)
	{
		output += itos(p_context.instructions[t]->opcode) + ",";
		switch (wc::bytecode::getOpCount(p_context.instructions[t]->opcode))
		{
		case 1:
			output += itos(static_pointer_cast<wcInstructionPlusOperand>(p_context.instructions[t])->operand1) + ",";
			break;
		case 2:
			output += itos(static_pointer_cast<wcInstructionPlusOperands>(p_context.instructions[t])->operand1) + ",";
			output += itos(static_pointer_cast<wcInstructionPlusOperands>(p_context.instructions[t])->operand2) + ",";
			break;
		}
	}
	return exportSource(p_outputFilename, output);
}

bool incIndex(vector<string>& source, int& x, int& y)
{
	++y;
	if (y == source[x].size())
	{
		++x; y = 0;
		if (x == source.size())
			return false;
	}
	return true;
}

bool wc::io::importBytecode(wc::bytecode::wcExecContext &p_outputContext, std::string inputFilename)
{
	auto source = importSource(inputFilename);

	for (int t = 0; t < source.size(); ++t)
		for (int y = 0; y < source[t].size(); ++y)
			switch (getOpCount(wcOpcode(util::stoi(source[t].substr(y, 1)))))
			{
			case 0:
				p_outputContext.instructions.push_back((make_shared<wcInstruction>(wcInstruction(wcOpcode(util::stoi(source[t].substr(y, 1)))))));
			}

	bool endOfInput = false;
	int line = 0, col = 0, currentValue = 0, operand1, operand2;
	while (!endOfInput)
	{
		currentValue = util::stoi(source[line].substr(col, 1));
		switch (getOpCount(wcOpcode(currentValue)))
		{
		case 0:
			p_outputContext.instructions.push_back(make_shared<wcInstruction>(wcInstruction(wcOpcode(currentValue))));
			break;
		case 1:
			if (incIndex(source, line, col))
				return false;	//eos whilst reading 1st operand
			operand1 = util::stoi(source[line].substr(col, 1));
			p_outputContext.instructions.push_back(make_shared<wcInstructionPlusOperand>(wcInstructionPlusOperand(wcOpcode(currentValue), operand1)));
			break;
		case 2:
			if (incIndex(source, line, col))
				return false;	//eos whilst reading 1st operand

			operand1 = util::stoi(source[line].substr(col, 1));
			if (incIndex(source, line, col))
				return false;	//eos whilst reading 2nd operand

			operand2 = util::stoi(source[line].substr(col, 1));
			p_outputContext.instructions.push_back(make_shared<wcInstructionPlusOperands>(wcInstructionPlusOperands(wcOpcode(currentValue), operand1, operand2)));
			break;
		}
	}

	return true;
}
