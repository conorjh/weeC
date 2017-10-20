#include "simple.h"
#include "codegen.h"
#include "parse.h"
#include <vector>

using namespace std;
using namespace wc;
using namespace wc::bytecode;
using namespace wc::parse;
using namespace wc::api;
using namespace wc::lex;
using namespace wc::codegen;
namespace wc
{
	namespace codegen
	{
		vector<wcToken> genSimpExpression_genRPN(wcParseIndex& p_index, int startingDepth, vector<wcToken>* rpnOutput);
		vector<wcInstruction> genSimpExpression_genInstructionsFromRPN(wcParseIndex& p_index);
	}
}

vector<wcInstruction> wc::codegen::genSimpExpression(wcParseIndex& p_index, vector<wcToken>* rpnOutput = nullptr)
{
	if (p_index.getNode()->type != pn_exp)
		return vector<wcInstruction>();
	int startingDepth = p_index.getNodeDepth(p_index.getNode());
	p_index.nextNode();
	if (rpnOutput == nullptr)
		rpnOutput = &vector<wcToken>();	

	//output rpn to rpnOutput
	genSimpExpression_genRPN(p_index, startingDepth, rpnOutput);

	//emit simpcode based on the RPN
	vector<wcInstruction> output = genSimpExpression_genInstructionsFromRPN(p_index);

	//return
	return output;
}

vector<wcInstruction> wc::codegen::genSimpExpression_genInstructionsFromRPN(wcParseIndex& p_index)
{
	vector<wcInstruction> output;



	return output;
}

vector<wcToken> wc::codegen::genSimpExpression_genRPN(wcParseIndex& p_index, int startingDepth, vector<wcToken>* rpnOutput)
{
	vector<wcToken> stack;
	wcToken currentToken;
	bool foundParen = false;

	//increment through nodes creating a RPN expression
	while (p_index.getCurrentNodeDepth() > startingDepth)
	{
		currentToken = p_index.getNode()->tokens[0];
		switch (currentToken.type)
		{
		case pn_exp:
			genSimpExpression(p_index, rpnOutput);
			break;

		case pn_ident:	case pn_varident:	case pn_funcident:	case pn_funccall:
		CASE_ALL_LITERALS_PN
			rpnOutput->push_back(currentToken);
			break;

		CASE_ALL_OPERATORS_PN
			while (stack.size() && (getPrecedence(currentToken) == getPrecedence(stack[stack.size() - 1]) && !isRightAssociative(currentToken)
				|| isRightAssociative(currentToken) && getPrecedence(currentToken) < getPrecedence(stack[stack.size() - 1])))
			{
				rpnOutput->push_back(stack[stack.size() - 1]);
				stack.pop_back();
			}
			stack.push_back(currentToken);
			break;

		case pn_oparen:
			stack.push_back(currentToken);
			break;

		case pn_cparen:
			while (!foundParen && stack.size())
				if (stack[stack.size() - 1].type == pn_oparen)
					foundParen = true;
				else
				{
					rpnOutput->push_back(stack[stack.size() - 1]);
					stack.pop_back();
				}

			if (!foundParen)
				return *rpnOutput;	//error
			stack.pop_back();

			//func ident? copied from old version
			if (stack.size() && stack[stack.size() - 1].type == pn_funcident)
			{
				rpnOutput->push_back(stack[stack.size() - 1]);
				stack.pop_back();
			}
			break;

		default:

			break;
		}
		p_index.nextNode();
	}

	//pop off remaining operators
	while (stack.size() > 0)
	{
		rpnOutput->push_back(stack[stack.size() - 1]);
		stack.pop_back();
	}

	return *rpnOutput;
}

int wc::codegen::genSimpIf(wcParseIndex& p_index)
{

	return 1;
}

int wc::codegen::genSimpDecVar(wcParseIndex& p_index)
{
	wcToken typeToken,identToken;
	vector<wcInstruction> expressionInstructions;
	int startingDepth = p_index.getNodeDepth(p_index.getNode());
	p_index.nextNode();

	while (p_index.getCurrentNodeDepth() > startingDepth)
		switch (p_index.getNode()->type)
		{
		case pn_type:
			typeToken = p_index.getNode()->tokens[0];
			p_index.nextNode();
			break;

		case pn_ident:
			identToken = p_index.getNode()->tokens[0];
			p_index.nextNode();
			break;

		case pn_exp:
			expressionInstructions = genSimpExpression(p_index);
			break;

		default:
			//error
			return 0;
		}

	return 1;
}

int wc::codegen::genSimpStatement(wcParseIndex& p_index)
{
	if (p_index.getNode()->type != pn_statement)
		return 0;
	p_index.nextNode();

	switch (p_index.getNode()->type)
	{
	case pn_exp:
		genSimpExpression(p_index);
		break;

	case pn_if:
		genSimpIf(p_index);
		break;

	case pn_decvar:
		genSimpDecVar(p_index);
		break;
	}

	return 1;
}

namespace wc
{
	namespace codegen
	{
		wcParseIndex gen_initParseIndex(wcParseIndex p_pi, wcAST& p_ast);
	}
}

//increment past the head to the first statement
wcParseIndex wc::codegen::gen_initParseIndex(wcParseIndex p_pi, wcAST& p_ast)
{
	p_pi.setNode(&p_ast.parseTree, p_ast.parseTree.begin());

	if (p_pi.getNode()->type == pn_head)
		p_pi.nextNode();
	else
		return p_pi;

	while (p_pi.getNode()->type != pn_statement)
		p_pi.nextNode();
	return p_pi;
}

int wc::codegen::genSimpExpression(wcParseIndex& p_index)
{
	//increment through nodes creating a RPN expression
	int startingDepth = p_index.getNodeDepth(p_index.getNode());

	//emit simpcode based on the RPN


	return 1;
}

int wc::codegen::genSimpIf(wcParseIndex& p_index)
{

	return 1;
}

int wc::codegen::genSimpDecVar(wcParseIndex& p_index)
{
	wcToken typeToken,identToken;
	
	int startingDepth = p_index.getNodeDepth(p_index.getNode());
	p_index.nextNode();
	while (p_index.getCurrentNodeDepth() > startingDepth)
	{
		switch (p_index.getNode()->type)
		{
		case pn_type:
			typeToken = p_index.getNode()->tokens[0];
			break;

		case pn_ident:
			identToken = p_index.getNode()->tokens[0];
			break;

		case pn_exp:

			break;

		default:
			//error
			return 0;
		}
		p_index.nextNode();
	}

	//generate instructions

	return 1;
}

int wc::codegen::genSimpStatement(wcParseIndex& p_index)
{
	if (p_index.getNode()->type != pn_statement)
		return 0;
	p_index.nextNode();

	switch (p_index.getNode()->type)
	{
	case pn_exp:
		genSimpExpression(p_index);
		break;

	case pn_if:
		genSimpIf(p_index);
		break;

	case pn_decvar:
		genSimpDecVar(p_index);
		break;
	}

	return 1;
}

namespace wc
{
	namespace codegen
	{
		wcParseIndex gen_initParseIndex(wcParseIndex p_pi, wcAST& p_ast);
	}
}

//increment past the head to the first statement
wcParseIndex wc::codegen::gen_initParseIndex(wcParseIndex p_pi, wcAST& p_ast)
{
	p_pi.setNode(&p_ast.parseTree, p_ast.parseTree.begin());

	if (p_pi.getNode()->type == pn_head)
		p_pi.nextNode();
	else
		return p_pi;

	while (p_pi.getNode()->type != pn_statement)
		p_pi.nextNode();
	return p_pi;
}

wcExecContext wc::codegen::wcSimpleBytecodeGen::gen(wcAST& p_ast)
{
	wcExecContext output;
	output.target = ct_simple_bytecode;

	wcParseIndex pindex = gen_initParseIndex(wcParseIndex(), p_ast);

	while (pindex.getNode() != p_ast.parseTree.end())
		genSimpStatement(pindex);

	return output;
}

wc::vm::wcSimpleVM::wcSimpleVM()
{

}

int wc::vm::wcSimpleVM::execInstruction(wcExecContext& p_context, wcInstruction p_instruction)
{
	switch (p_instruction.opCode)
	{
		case oc_nop:

			break;
	}
	return 1;
}

wc::compile::wcSimpleCompiler::wcSimpleCompiler()
{

}

wcScript wc::compile::wcSimpleCompiler::compile(vector<string> p_source)
{
	wcScript output;

	//lex
	wcLexer lex;
	vector<wcToken> tokens = lex.lex(p_source);
	if (lex.getError().code)
		return wcScript(lex.getError());

	//parse
	wcParser parser;
	wcAST tree = parser.parse(tokens);
	if (parser.getError().code)
		return wcScript(parser.getError());

	//code gen
	wcSimpleBytecodeGen gen;
	output.con = gen.gen(tree);
	if (gen.getError().code)
		return wcScript(gen.getError());

	//return what we've created
	return output;
}

