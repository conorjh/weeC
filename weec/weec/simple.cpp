#include "simple.h"
#include "codegen.h"
#include "parse.h"
#include "util.h"
#include <vector>

using namespace std;
using namespace wc;
using namespace wc::bytecode;
using namespace wc::parse;
using namespace wc::api;
using namespace wc::lex;
using namespace wc::codegen;
using namespace wc::util;
using namespace wc::vm;

namespace wc
{
	namespace codegen
	{
		inline vector<wcParseNode> genSimpExpression_genRPN(wcParseIndex& p_index, bytecode::wcExecContext& output, int startingDepth, vector<wcParseNode>* rpnOutput);
		inline vector<wcInstruction> genSimpExpression_genInstructionsFromRPN(wcParseIndex& p_index, bytecode::wcExecContext& output, vector<wcParseNode>* rpnOutput);
	}
}

vector<wcInstruction> wc::codegen::genSimpExpression(wcParseIndex& p_index, bytecode::wcExecContext& output, vector<wcParseNode>* rpnOutput = nullptr)
{
	if (p_index.getNode()->type != pn_exp)
		return vector<wcInstruction>();
	if (rpnOutput == nullptr)
		rpnOutput = &vector<wcParseNode>();	

	int startingDepth = p_index.getNodeDepth(p_index.getNode());
	p_index.nextNode();

	//output rpn to rpnOutput
	genSimpExpression_genRPN(p_index, output, startingDepth, rpnOutput);

	//emit simpcode based on the RPN
	vector<wcInstruction> instructionOutput = genSimpExpression_genInstructionsFromRPN(p_index, output, rpnOutput);

	return instructionOutput;
}

vector<wcParseNode> wc::codegen::genSimpExpression_genRPN(wcParseIndex& p_index, bytecode::wcExecContext& output, int startingDepth, vector<wcParseNode>* rpnOutput)
{
	vector<wcParseNode> stack;
	wcToken currentToken,topOfStack;
	bool foundParen = false;

	//increment through nodes creating a RPN expression
	while (p_index.getCurrentNodeDepth() > startingDepth)
	{
		currentToken = p_index.getNode()->tokens[0];	topOfStack = stack[stack.size() - 1].tokens[0];
		switch (p_index.getNode()->type)
		{
		case pn_exp:
			genSimpExpression(p_index, output, rpnOutput);
			break;

		case pn_ident:	case pn_varident:	case pn_funcident:	case pn_funccall:
		CASE_ALL_LITERALS_PN
			rpnOutput->push_back(p_index.getNode().node->data);
			break;

		CASE_ALL_OPERATORS_PN
			while (stack.size() && (getPrecedence(currentToken) == getPrecedence(topOfStack) && !isRightAssociative(currentToken)
				|| isRightAssociative(currentToken) && getPrecedence(currentToken) < getPrecedence(topOfStack)))
			{
				rpnOutput->push_back(stack[stack.size() - 1]);
				stack.pop_back();
			}
			stack.push_back(p_index.getNode().node->data);
			break;

		case pn_oparen:
			stack.push_back(p_index.getNode().node->data);
			break;

		case pn_cparen:
			while (!foundParen && stack.size())
				if (topOfStack.type == pn_oparen)
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

vector<wcInstruction> wc::codegen::genSimpExpression_genInstructionsFromRPN(wcParseIndex& p_index, bytecode::wcExecContext& output, vector<wcParseNode>* rpnOutput)
{
	vector<wcInstruction> instructionOutput;
	int operand1,operand2; 
	string stringLiteral;

	for (int i = 0; i < rpnOutput->size(); ++i)
		switch(rpnOutput->at(i).type)
		{
		//literals
		case pn_strlit:
			stringLiteral = rpnOutput->at(i).tokens[0].data;
			if (!output.stringTable.doesStringExist(stringLiteral))
				operand1 = output.stringTable.addEntry(stringLiteral);	//add to string table if it's the first occurence
			else
				operand1 = output.stringTable.getIndex(stringLiteral);	//we've seen this before, retrieve index from string table
			instructionOutput.push_back(wcInstructionPlusOperand(soc_push, operand1));
			break;

		case pn_intlit:
			operand1 = stoi(rpnOutput->at(i).tokens[0].data);
			instructionOutput.push_back(wcInstructionPlusOperand(soc_push, operand1));
			break;

		case pn_fltlit:
			break;

		case pn_true:
			instructionOutput.push_back(wcInstructionPlusOperand(soc_push, 1));
			break;
			
		case pn_false:
			instructionOutput.push_back(wcInstructionPlusOperand(soc_push, 0));
			break;

		//variables
		case pn_ident:
		case pn_varident:
		case pn_funcident:
		case pn_funccall:
			break;

		//operators
		case pn_lognot:
			instructionOutput.push_back(wcInstruction(soc_not));	
			break;
		case pn_logand:
			instructionOutput.push_back(wcInstruction(soc_and));	
			break;
		case pn_logor:
			instructionOutput.push_back(wcInstruction(soc_or));		
			break;
		case pn_equal:
			instructionOutput.push_back(wcInstruction(soc_cmp));
			instructionOutput.push_back(wcInstructionPlusOperand(soc_jne, instructionOutput.size() + 3));
			instructionOutput.push_back(wcInstructionPlusOperand(soc_push, 1));
			instructionOutput.push_back(wcInstructionPlusOperand(soc_jmp, instructionOutput.size() + 2));
			instructionOutput.push_back(wcInstructionPlusOperand(soc_push, 0));
			break;
		case pn_notequal:
			instructionOutput.push_back(wcInstruction(soc_cmp));
			instructionOutput.push_back(wcInstructionPlusOperand(soc_je, instructionOutput.size() + 3));
			instructionOutput.push_back(wcInstructionPlusOperand(soc_push, 1));
			instructionOutput.push_back(wcInstructionPlusOperand(soc_jmp, instructionOutput.size() + 2));
			instructionOutput.push_back(wcInstructionPlusOperand(soc_push, 0));
			break;
		case pn_greater:
			instructionOutput.push_back(wcInstruction(soc_cmp));
			instructionOutput.push_back(wcInstructionPlusOperand(soc_jle, instructionOutput.size() + 3));
			instructionOutput.push_back(wcInstructionPlusOperand(soc_push, 1));
			instructionOutput.push_back(wcInstructionPlusOperand(soc_jmp, instructionOutput.size() + 2));
			instructionOutput.push_back(wcInstructionPlusOperand(soc_push, 0));
			break;
		case pn_greaterequal:
			instructionOutput.push_back(wcInstruction(soc_cmp));
			instructionOutput.push_back(wcInstructionPlusOperand(soc_jl, instructionOutput.size() + 3));
			instructionOutput.push_back(wcInstructionPlusOperand(soc_push, 1));
			instructionOutput.push_back(wcInstructionPlusOperand(soc_jmp, instructionOutput.size() + 2));
			instructionOutput.push_back(wcInstructionPlusOperand(soc_push, 0));
			break;

		case pn_assign:
		case pn_mult:
			instructionOutput.push_back(wcInstruction(soc_mult));
			break;
		case pn_div:
			instructionOutput.push_back(wcInstruction(soc_div));
			break;
		case pn_plus:
			instructionOutput.push_back(wcInstruction(soc_plus));
			break;
		case pn_minus:
			instructionOutput.push_back(wcInstruction(soc_minus));
			break;
		case pn_mod:
			instructionOutput.push_back(wcInstruction(soc_mod));
			break;
		case pn_expo:
			instructionOutput.push_back(wcInstruction(soc_expo));
			break;
		default:
			break;
		}

	return instructionOutput;
}

int wc::codegen::genSimpIf(wcParseIndex& p_index, bytecode::wcExecContext& output)
{

	return 1;
}

int wc::codegen::genSimpDecVar(wcParseIndex& p_index, bytecode::wcExecContext& output)
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
			expressionInstructions = genSimpExpression(p_index, output);
			break;

		default:
			//error
			return 0;
		}

	return 1;
}

int wc::codegen::genSimpStatement(wcParseIndex& p_index, wcExecContext& output)
{
	if (p_index.getNode()->type != pn_statement)
		return 0;
	p_index.nextNode();

	switch (p_index.getNode()->type)
	{
	case pn_exp:
		genSimpExpression(p_index, output);
		break;

	case pn_if:
		genSimpIf(p_index, output);
		break;

	case pn_decvar:
		genSimpDecVar(p_index, output);
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
	output.targetPlatform = ct_simple_bytecode;

	wcParseIndex pindex = gen_initParseIndex(wcParseIndex(), p_ast);

	while (pindex.getNode() != p_ast.parseTree.end())
		genSimpStatement(pindex,output);

	return output;
}

wc::vm::wcSimpleVM::wcSimpleVM()
{

}

int wc::vm::wcSimpleVM::execInstruction(wcExecContext& p_context, wcInstruction p_instruction)
{
	switch (p_instruction.opcode)
	{
		case soc_nop:
			break;
		case soc_push:
			exec_s_push(p_context, static_cast<wcInstructionPlusOperand&>(p_instruction));
			break;
		case soc_pop:
			exec_s_pop(p_context,p_instruction);
			break;
		case soc_cmp: 
			exec_s_cmp(p_context, p_instruction);
			break;

		case soc_jmp: 
		case soc_je: 
		case soc_jne: 
		case soc_jg: 
		case soc_jl: 
		case soc_jge: 
		case soc_jle:
			break;
		//stack math/logical operations
		case soc_assign: case soc_plus: case soc_minus: case soc_mult: case soc_div: case soc_expo: case soc_mod: case soc_inc: case soc_dec:
		case soc_and: case soc_or: case soc_xor: case soc_not: case soc_shfl: case soc_shfr:
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

void wc::vm::exec_s_push(wcExecContext &p_context, wcInstructionPlusOperand p_instr)
{
	p_context.stack.push(p_instr.operand1);
}

void wc::vm::exec_s_pop(wcExecContext &p_context, wcInstruction p_instr)
{
	p_context.stack.pop();
}

void wc::vm::exec_s_cmp(wcExecContext &p_context, wcInstruction p_instr)
{
	p_context.registers.t2 = p_context.stack.pop();
	p_context.registers.t1 = p_context.stack.pop();
	p_context.registers.cmp = p_context.registers.t1 - p_context.registers.t2;
}
