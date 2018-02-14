#include "codegen.h"
#include "simple.h"
#include "tests.h"
#include <vector>
#include <iostream>
using namespace wc;
using namespace wc::parse;
using namespace wc::lex;
using namespace wc::codegen;
using namespace wc::bytecode;
using namespace wc::vm;
using namespace wc::error;
using namespace std;

namespace wctest
{
	namespace exp
	{
		int standardExpTest(const char*);
		int standardExpTest(const char*, const char* expectedResult);
		void printExpInstructions(vector<wcParseNode>* p_instructions);

	}
}

void wctest::exp::printExpInstructions(vector<wcParseNode>* p_instructions)
{
	wcToken tok;
	for (int t = 0; t < p_instructions->size(); ++t)
	{
		cout << p_instructions->at(t).tokens[0].data << " ";
	}
	cout << endl;
}

int wctest::exp::standardExpTest(const char * p_source)
{
	wcLexer lexer;
	auto tokens = lexer.lex(p_source);
	if (lexer.getError().code > 0)
		return lexer.getError().code;

	wcParser parser;
	wcAST ast;
	ast = parser.parse(tokens);
	if (parser.getError().code)
		return parser.getError().code;

	//generate expression instructions
	wcSimpleExecContext output;
	output.targetPlatform = ct_simple_bytecode;
	wcParseIndex pindex = gen_initParseIndex(wcParseIndex(), ast);
	wcGenParams params(pindex, ast, output);
	vector<wcParseNode> rpnOutput;
	while (pindex.getNode() != ast.parseTree.end())
	{
		params.pindex.nextNode();
		switch (params.pindex.getNode()->type)
		{
		case pn_exp:
			genSimpExpression(params,&rpnOutput);
			break;

		default:
			params.pindex.nextNode();
			break;
		}
	}

	cout << p_source << endl;
	printExpInstructions(&rpnOutput);

	return 1;
}

int wctest::exp::standardExpTest(const char * p_source, const char * expectedOutput)
{
	wcLexer lexer;
	auto tokens = lexer.lex(p_source);
	if (lexer.getError().code > 0)
		return lexer.getError().code;

	wcParser parser;
	wcAST ast;
	ast = parser.parse(tokens);
	if (parser.getError().code)
		return parser.getError().code;

	//generate expression instructions
	wcSimpleExecContext output;
	output.targetPlatform = ct_simple_bytecode;
	wcParseIndex pindex = gen_initParseIndex(wcParseIndex(), ast);
	wcGenParams params(pindex, ast, output);
	vector<wcParseNode> rpnOutput;
	while (pindex.getNode() != ast.parseTree.end())
	{
		params.pindex.nextNode();
		switch (params.pindex.getNode()->type)
		{
		case pn_exp:
			genSimpExpression(params, &rpnOutput);
			break;

		default:
			params.pindex.nextNode();
			break;
		}
	}

	cout << p_source << endl;
	printExpInstructions(&rpnOutput);

	return (p_source == expectedOutput);
}

int wctest::exp::exp_basic_1()
{
	return standardExpTest("123 * 345 + 678;", "123 345 * 678 +");
}

int wctest::exp::exp_basic_2()
{
	return standardExpTest("123 * 345 + 678 / 2 ^ 3;", "123 345 * 678 2 3 ^ / +");
}
