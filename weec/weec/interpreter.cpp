#include "interpreter.h"

using enum weec::parse::wcParseNodeType;

using namespace std;
using namespace weec::lex;
using namespace weec::parse;

void weec::interpreter::wcExpressionInterpeter::SetupImplementationTypeNames()
{
	ImplementationTypes.insert(make_pair("int", ImplementationType("int", std::type_index(typeid(int)))));
	ImplementationTypes.insert(make_pair("unsigned int", ImplementationType("unsigned int", std::type_index(typeid(unsigned int)))));
	ImplementationTypes.insert(make_pair("float", ImplementationType("float", std::type_index(typeid(float)))));
	ImplementationTypes.insert(make_pair("double", ImplementationType("double", std::type_index(typeid(double)))));
	ImplementationTypes.insert(make_pair("bool", ImplementationType("bool", std::type_index(typeid(bool)))));
}

std::any weec::interpreter::wcExpressionInterpeter::EvalNode(parse::wcParseNodeType Type, parse::wcParseNodeType CalledFrom)
{
	switch (PC->Type)
	{
	case Expression:
		return ExecSubExpression();
	case Expression_Equality:
		return ExecEquality();
	case Expression_Assignment:
		return ExecAssignment();
	case Expression_LogicOr:
		return ExecLogicOr();
	case Expression_LogicAnd:
		return ExecLogicAnd();
	case Expression_Comparison:
		return ExecComparison();
	case Expression_Term:
		return ExecTerm();
	case Expression_Factor:
		return ExecFactor();
	case Expression_Unary:
		return ExecUnary();
	case Expression_Primary:
		return ExecPrimary();
	case Expression_Operator:
		return ExecOperator();
	default:
		//error
		break;
	}

	return std::any();
}

std::any weec::interpreter::wcExpressionInterpeter::DoOp(lex::wcTokenType Op, std::any a, std::any b)
{
	auto FindA = ImplementationTypes.find(a.type().name());
	auto FindB = ImplementationTypes.find(b.type().name());
	if (FindA == ImplementationTypes.end() || FindB == ImplementationTypes.end())
		return std::any();

	if (FindA->first == "int")
	{
		if (FindB->first == "int")
			return AnyOperator<int, int>().DoOp(Op, a, b);
		else if (FindB->first == "unsigned int")
			return AnyOperator<int, unsigned int>().DoOp(Op, a, b);
		else if (FindB->first == "float")
			return AnyOperator<int, float>().DoOp(Op, a, b);
		else if (FindB->first == "double")
			return AnyOperator<int, double>().DoOp(Op, a, b);
		else if (FindB->first == "bool")
			return AnyOperator<int, bool>().DoOp(Op, a, b);
	}
	else if (FindA->first == "unsigned int")
	{
		if (FindB->first == "int")
			return AnyOperator<unsigned int, int>().DoOp(Op, a, b);
		else if (FindB->first == "unsigned int")
			return AnyOperator<unsigned int, unsigned int>().DoOp(Op, a, b);
		else if (FindB->first == "float")
			return AnyOperator<unsigned int, float>().DoOp(Op, a, b);
		else if (FindB->first == "double")
			return AnyOperator<unsigned int, double>().DoOp(Op, a, b);
		else if (FindB->first == "bool")
			return AnyOperator<unsigned int, bool>().DoOp(Op, a, b);
	}
	else if (FindA->first == "float")
	{
		if (FindB->first == "int")
			return AnyOperator<float, int>().DoOp(Op, a, b);
		else if (FindB->first == "unsigned int")
			return AnyOperator<float, unsigned int>().DoOp(Op, a, b);
		else if (FindB->first == "float")
			return AnyOperator<float, float>().DoOp(Op, a, b);
		else if (FindB->first == "double")
			return AnyOperator<float, double>().DoOp(Op, a, b);
		else if (FindB->first == "bool")
			return AnyOperator<float, bool>().DoOp(Op, a, b);
	}
	else if (FindA->first == "double")
	{
		if (FindB->first == "int")
			return AnyOperator<double, int>().DoOp(Op, a, b);
		else if (FindB->first == "unsigned int")
			return AnyOperator<double, unsigned int>().DoOp(Op, a, b);
		else if (FindB->first == "float")
			return AnyOperator<double, float>().DoOp(Op, a, b);
		else if (FindB->first == "double")
			return AnyOperator<double, double>().DoOp(Op, a, b);
		else if (FindB->first == "bool")
			return AnyOperator<double, bool>().DoOp(Op, a, b);
	}
	else if (FindA->first == "bool")
	{
		if (FindB->first == "int")
			return AnyOperator<bool, int>().DoOp(Op, a, b);
		else if (FindB->first == "unsigned int")
			return AnyOperator<bool, unsigned int>().DoOp(Op, a, b);
		else if (FindB->first == "float")
			return AnyOperator<bool, float>().DoOp(Op, a, b);
		else if (FindB->first == "double")
			return AnyOperator<bool, double>().DoOp(Op, a, b);
		else if (FindB->first == "bool")
			return AnyOperator<bool, bool>().DoOp(Op, a, b);
	}
	return std::any();
}

std::any weec::interpreter::wcExpressionInterpeter::DoOp(lex::wcTokenType Op, std::any a)
{
	auto FindA = ImplementationTypes.find(a.type().name());
	if (FindA == ImplementationTypes.end())
		return std::any();

	if (FindA->first == "int")
		return AnyOperatorUnary<int>().DoOp(Op, a);
	else if (FindA->first == "unsigned int")
		return AnyOperatorUnary<unsigned int>().DoOp(Op, a);
	else if (FindA->first == "float")
		return AnyOperatorUnary<float>().DoOp(Op, a);
	else if (FindA->first == "double")
		return AnyOperatorUnary<double>().DoOp(Op, a);
	else if (FindA->first == "bool")
		return AnyOperatorUnary<bool>().DoOp(Op, a);
	return std::any();
}



weec::interpreter::wcExpressionInterpeter::wcExpressionInterpeter(parse::wcParseSymbolTable& _SymTab, parse::wcParseOutput _Input, tree<parse::wcParseNode>::iterator _PC)
	: Input(_Input), SymTab(_SymTab)
{
	PC = _PC;
	SetupImplementationTypeNames();
}

std::any weec::interpreter::wcExpressionInterpeter::ExecSubExpression()
{
	PC++;
	return EvalNode(PC->Type, Expression);
}

std::any weec::interpreter::wcExpressionInterpeter::ExecOperator()
{
	return EvalNode(PC->Type, Expression);
}

std::any weec::interpreter::wcExpressionInterpeter::Exec()
{
	std::any ExpressionResult;

	while (PC.node != nullptr)
	{
		ExpressionResult = EvalNode(PC->Type, Expression);

		PC++;
	}

	return ExpressionResult;
}

std::any weec::interpreter::wcExpressionInterpeter::ExecPrimary()
{
	auto Result = PC++;

	switch (Result->Token.Type)
	{
	case wcTokenType::IntLiteral:
		return stoi(Result->Token.StringToken.Data);
	case wcTokenType::StringLiteral:
		return Result->Token.StringToken.Data;
	case wcTokenType::FloatLiteral:
		return stof(Result->Token.StringToken.Data);
	case wcTokenType::Identifier:
		return stof(Result->Token.StringToken.Data);
	}
	return std::any();
}

std::any weec::interpreter::wcExpressionInterpeter::ExecUnary()
{
	auto OpType = PC->Token.Type;
	PC++;

	auto Lh = EvalNode(PC->Type, Expression);

	return DoOp(OpType, Lh);
}

std::any weec::interpreter::wcExpressionInterpeter::ExecFactor()
{
	auto OpType = PC->Token.Type;
	PC++;

	auto Lh = EvalNode(PC->Type, Expression);
	auto Rh = EvalNode(PC->Type, Expression);

	return DoOp(OpType, Lh, Rh);
}

std::any weec::interpreter::wcExpressionInterpeter::ExecTerm()
{
	auto OpType = PC->Token.Type;
	PC++;

	auto Lh = EvalNode(PC->Type, Expression);
	auto Rh = EvalNode(PC->Type, Expression);

	return DoOp(OpType, Lh, Rh);
}

std::any weec::interpreter::wcExpressionInterpeter::ExecComparison()
{
	auto OpType = PC->Token.Type;
	PC++;

	auto Lh = EvalNode(PC->Type, Expression);
	auto Rh = EvalNode(PC->Type, Expression);

	return DoOp(OpType, Lh, Rh);
}

std::any weec::interpreter::wcExpressionInterpeter::ExecLogicAnd()
{
	auto OpType = PC->Token.Type;
	PC++;

	auto Lh = EvalNode(PC->Type, Expression);
	auto Rh = EvalNode(PC->Type, Expression);

	return DoOp(OpType, Lh, Rh);
}

std::any weec::interpreter::wcExpressionInterpeter::ExecLogicOr()
{
	auto OpType = PC->Token.Type;
	PC++;

	auto Lh = EvalNode(PC->Type, Expression);
	auto Rh = EvalNode(PC->Type, Expression);

	return DoOp(OpType, Lh, Rh);
}

std::any weec::interpreter::wcExpressionInterpeter::ExecAssignment()
{
	auto OpType = PC->Token.Type;
	PC++;

	auto Lh = EvalNode(PC->Type, Expression);
	auto Rh = EvalNode(PC->Type, Expression);

	return DoOp(OpType, Lh, Rh);
}

std::any weec::interpreter::wcExpressionInterpeter::ExecEquality()
{
	auto OpType = PC->Token.Type;
	PC++;

	auto Lh = EvalNode(PC->Type, Expression);
	auto Rh = EvalNode(PC->Type, Expression);

	return DoOp(OpType, Lh, Rh);
}

weec::interpreter::wcInterpreter::wcInterpreter(parse::wcParseOutput _Input)
	: ExpressionInterp(SymbolTable, _Input, PC)
{
	Input = _Input;
}

void weec::interpreter::wcInterpreter::Reset()
{
}

std::any weec::interpreter::wcInterpreter::Exec()
{
	PC = Input.AST.begin();

	std::any ExpressionResult, ReturnResult;
	while (PC != Input.AST.end() && PC != nullptr && !Halt)
	{
		switch (PC->Type)
		{
		case Statement:
			ExecStatement();
			break;

		case Block:
			ExecBlock();
			break;

		case Head:
			break;

		default:
			Error.Node = *PC;
			Error.Code = wcIntpreterErrorCode::InvalidNode;
			Halt = true;
			break;
		}

		PC++;
	}

	return ExpressionResult;
}

std::any weec::interpreter::wcInterpreter::ExecStatement()
{
	auto Begin = PC;
	PC++;

	auto d1 = Input.AST.depth(Begin);
	auto d2 = Input.AST.depth(PC);
	auto d3 = PC->Type;
	if(Input.AST.depth(PC) > Input.AST.depth(Begin))
		switch (PC->Type)
		{
		case Expression:
			LastExpression = wcExpressionInterpeter(SymbolTable, Input, PC).Exec();
			break;

		case ReturnStatement:
			Return = ExecReturn();
			break;

		case IfStatement:
			ExecIf();
			break;

		case Declaration:
			ExecDeclaration();
			break;

		default:
			Error.Node = *PC;
			Error.Code = wcIntpreterErrorCode::InvalidNode;
			Halt = true;
			break;
		}

	return Return;
}

std::any weec::interpreter::wcInterpreter::ExecReturn()
{
	auto Begin = PC;
	PC++;

	while (Input.AST.depth(PC) > Input.AST.depth(Begin) && (PC != Input.AST.end() && !Halt))
		switch (PC->Type)
		{
		case Expression:
			Return = wcExpressionInterpeter(SymbolTable, Input, PC).Exec();
			break;

		default:
			Error.Node = *PC;
			Error.Code = wcIntpreterErrorCode::InvalidNode;
			Halt = true;
			break;
		}

	Halt = true;
	return std::any();
}

std::any weec::interpreter::wcInterpreter::ExecBlock()
{
	auto Begin = PC;
	PC++;

	while (Input.AST.depth(PC) > Input.AST.depth(Begin) && (PC != Input.AST.end() && !Halt))
		switch (PC->Type)
		{
		case Statement:
			ExecStatement();
			break;

		default:
			Error.Node = *PC;
			Error.Code = wcIntpreterErrorCode::InvalidNode;
			Halt = true;
			break;
		}

	return std::any();
}

std::any weec::interpreter::wcInterpreter::ExecIf()
{
	return std::any();
}

std::any weec::interpreter::wcInterpreter::ExecDeclaration()
{
	auto Begin = PC;
	PC++;
	auto d1 = Input.AST.depth(Begin);
	auto d2 = Input.AST.depth(PC);
	auto d3 = PC->Type;

	while (Input.AST.depth(PC) > Input.AST.depth(Begin) && (PC != Input.AST.end() && PC != nullptr && !Halt))
		switch (PC++->Type)
		{
		case Declaration_Type:
			break;

		case Declaration_Ident:
			break;

		case Expression:
			LastExpression = wcExpressionInterpeter(SymbolTable, Input, PC).Exec();
			break;

		default:
			Error.Node = *PC;
			Error.Code = wcIntpreterErrorCode::InvalidNode;
			Halt = true;
			break;
		}

	return Return;
}
