#include "interpreter.h"

using enum weec::parse::wcParseNodeType;

using namespace std;
using namespace weec::lex;
using namespace weec::parse;

void weec::interpreter::wcExpressionInterpeter::SetupImplementationTypeNames()
{
	ImplementationTypeNames.insert(make_pair("int", std::type_index(typeid(int)).name()));
	ImplementationTypeNames.insert(make_pair("unsigned int", std::type_index(typeid(unsigned int)).name()));
	ImplementationTypeNames.insert(make_pair("float", std::type_index(typeid(float)).name()));
	ImplementationTypeNames.insert(make_pair("double", std::type_index(typeid(double)).name()));
}

std::any weec::interpreter::wcExpressionInterpeter::ChoosePath(parse::wcParseNodeType Type, parse::wcParseNodeType CalledFrom)
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

weec::interpreter::wcExpressionInterpeter::wcExpressionInterpeter(parse::wcParseSymbolTable& _SymTab, parse::wcParseOutput _Input, tree<parse::wcParseNode>::iterator _PC)
	: Input(_Input), SymTab(_SymTab)
{
	PC = _PC;
	SetupImplementationTypeNames();
}

std::any weec::interpreter::wcExpressionInterpeter::ExecSubExpression()
{
	PC++;
	return ChoosePath(PC->Type, Expression);
}

std::any weec::interpreter::wcExpressionInterpeter::ExecOperator()
{
	return ChoosePath(PC->Type, Expression);
}

std::any weec::interpreter::wcExpressionInterpeter::Exec()
{
	std::any ExpressionResult;

	while (PC.node != nullptr)
	{
		ExpressionResult = ChoosePath(PC->Type, Expression);

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
	}
	return std::any();
}

std::any weec::interpreter::wcExpressionInterpeter::ExecUnary()
{
	auto OpType = PC->Token.Type;
	PC++;

	auto Lh = ChoosePath(PC->Type, Expression);
	auto Rh = ChoosePath(PC->Type, Expression);

	switch (OpType)
	{
	case wcTokenType::PlusOperator:
		return std::any_cast<int>(Lh) + std::any_cast<int>(Rh);

	case wcTokenType::MinusOperator:
		return std::any_cast<int>(Lh) - std::any_cast<int>(Rh);
	}

	return std::any();
}

std::any weec::interpreter::wcExpressionInterpeter::ExecFactor()
{
	auto OpType = PC->Token.Type;
	PC++;

	auto Lh = ChoosePath(PC->Type, Expression);
	auto Rh = ChoosePath(PC->Type, Expression);
	string name;
	switch (OpType)
	{
	case wcTokenType::MultiplyOperator:
		if (string(Lh.type().name()) == ImplementationTypeNames["int"])
		{
			if (string(Rh.type().name()) == ImplementationTypeNames["int"])
				return std::any_cast<int>(Lh) * std::any_cast<int>(Rh);
			else if (string(Rh.type().name()) == ImplementationTypeNames["float"])
				return std::any_cast<int>(Lh) * std::any_cast<float>(Rh);
			else if (string(Rh.type().name()) == ImplementationTypeNames["unsigned int"])
				return std::any_cast<int>(Lh) * std::any_cast<unsigned int>(Rh);
		}
		else if (string(Lh.type().name()) == ImplementationTypeNames["float"])
		{
			if (string(Rh.type().name()) == ImplementationTypeNames["int"])
				return std::any_cast<float>(Lh) * std::any_cast<int>(Rh);
			else if (string(Rh.type().name()) == ImplementationTypeNames["float"])
				return std::any_cast<float>(Lh) * std::any_cast<float>(Rh);
			else if (string(Rh.type().name()) == ImplementationTypeNames["unsigned int"])
				return std::any_cast<float>(Lh) * std::any_cast<unsigned int>(Rh);
		}
		name = Rh.type().name();

		return std::any();	//error

	case wcTokenType::DivideOperator:
		if (string(Lh.type().name()) == ImplementationTypeNames["int"])
		{
			if (string(Rh.type().name()) == ImplementationTypeNames["int"])
				return std::any_cast<int>(Lh) / std::any_cast<int>(Rh);
			else if (string(Rh.type().name()) == ImplementationTypeNames["float"])
				return std::any_cast<int>(Lh) / std::any_cast<float>(Rh);
			else if (string(Rh.type().name()) == ImplementationTypeNames["unsigned int"])
				return std::any_cast<int>(Lh) / std::any_cast<unsigned int>(Rh);
		}
		else if (string(Lh.type().name()) == ImplementationTypeNames["float"])
		{
			if (string(Rh.type().name()) == ImplementationTypeNames["int"])
				return std::any_cast<float>(Lh) / std::any_cast<int>(Rh);
			else if (string(Rh.type().name()) == ImplementationTypeNames["float"])
				return std::any_cast<float>(Lh) / std::any_cast<float>(Rh);
			else if (string(Rh.type().name()) == ImplementationTypeNames["unsigned int"])
				return std::any_cast<float>(Lh) / std::any_cast<unsigned int>(Rh);
		}
		return std::any();	//error
	}

	return std::any();
}

std::any weec::interpreter::wcExpressionInterpeter::ExecTerm()
{
	auto OpType = PC->Token.Type;
	PC++;

	auto Lh = ChoosePath(PC->Type, Expression);
	auto Rh = ChoosePath(PC->Type, Expression);

	switch (OpType)
	{
	case wcTokenType::PlusOperator:
		if (string(Lh.type().name()) == ImplementationTypeNames["int"])
		{
			if (string(Rh.type().name()) == ImplementationTypeNames["int"])
				return std::any_cast<int>(Lh) + std::any_cast<int>(Rh);
			else if (string(Rh.type().name()) == ImplementationTypeNames["float"])
				return std::any_cast<int>(Lh) + std::any_cast<float>(Rh);
			else if (string(Rh.type().name()) == ImplementationTypeNames["unsigned int"])
				return std::any_cast<int>(Lh) + std::any_cast<unsigned int>(Rh);
		}
		else if (string(Lh.type().name()) == ImplementationTypeNames["float"])
		{
			if (string(Rh.type().name()) == ImplementationTypeNames["int"])
				return std::any_cast<float>(Lh) + std::any_cast<int>(Rh);
			else if (string(Rh.type().name()) == ImplementationTypeNames["float"])
				return std::any_cast<float>(Lh) + std::any_cast<float>(Rh);
			else if (string(Rh.type().name()) == ImplementationTypeNames["unsigned int"])
				return std::any_cast<float>(Lh) + std::any_cast<unsigned int>(Rh);
		}
		return std::any();	//error

	case wcTokenType::MinusOperator:
		if (string(Lh.type().name()) == ImplementationTypeNames["int"])
		{
			if (string(Rh.type().name()) == ImplementationTypeNames["int"])
				return std::any_cast<int>(Lh) - std::any_cast<int>(Rh);
			else if (string(Rh.type().name()) == ImplementationTypeNames["float"])
				return std::any_cast<int>(Lh) - std::any_cast<float>(Rh);
			else if (string(Rh.type().name()) == ImplementationTypeNames["unsigned int"])
				return std::any_cast<int>(Lh) - std::any_cast<unsigned int>(Rh);
		}
		else if (string(Lh.type().name()) == ImplementationTypeNames["float"])
		{
			if (string(Rh.type().name()) == ImplementationTypeNames["int"])
				return std::any_cast<float>(Lh) - std::any_cast<int>(Rh);
			else if (string(Rh.type().name()) == ImplementationTypeNames["float"])
				return std::any_cast<float>(Lh) - std::any_cast<float>(Rh);
			else if (string(Rh.type().name()) == ImplementationTypeNames["unsigned int"])
				return std::any_cast<float>(Lh) - std::any_cast<unsigned int>(Rh);
		}
		return std::any();
	}

	return std::any();
}

std::any weec::interpreter::wcExpressionInterpeter::ExecComparison()
{
	auto OpType = PC->Token.Type;
	PC++;

	auto Lh = ChoosePath(PC->Type, Expression);
	auto Rh = ChoosePath(PC->Type, Expression);

	switch (OpType)
	{
	case wcTokenType::PlusOperator:
		return std::any_cast<int>(Lh) + std::any_cast<int>(Rh);

	case wcTokenType::MinusOperator:
		return std::any_cast<int>(Lh) - std::any_cast<int>(Rh);
	}

	return std::any();
}

std::any weec::interpreter::wcExpressionInterpeter::ExecLogicAnd()
{
	auto OpType = PC->Token.Type;
	PC++;

	auto Lh = ChoosePath(PC->Type, Expression);
	auto Rh = ChoosePath(PC->Type, Expression);

	switch (OpType)
	{
	case wcTokenType::PlusOperator:
		if (string(Lh.type().name()) == ImplementationTypeNames["int"])
			return std::any_cast<int>(Lh) + std::any_cast<int>(Rh);

	case wcTokenType::MinusOperator:
		return std::any_cast<int>(Lh) - std::any_cast<int>(Rh);
	}

	return std::any();
}

std::any weec::interpreter::wcExpressionInterpeter::ExecLogicOr()
{
	auto OpType = PC->Token.Type;
	PC++;

	auto Lh = ChoosePath(PC->Type, Expression);
	auto Rh = ChoosePath(PC->Type, Expression);

	switch (OpType)
	{
	case wcTokenType::PlusOperator:
		return std::any_cast<int>(Lh) + std::any_cast<int>(Rh);

	case wcTokenType::MinusOperator:
		return std::any_cast<int>(Lh) - std::any_cast<int>(Rh);
	}

	return std::any();
}

std::any weec::interpreter::wcExpressionInterpeter::ExecAssignment()
{
	auto OpType = PC->Token.Type;
	PC++;

	auto Lh = ChoosePath(PC->Type, Expression);
	auto Rh = ChoosePath(PC->Type, Expression);

	switch (OpType)
	{
	case wcTokenType::PlusOperator:
		return std::any_cast<int>(Lh) + std::any_cast<int>(Rh);

	case wcTokenType::MinusOperator:
		return std::any_cast<int>(Lh) - std::any_cast<int>(Rh);
	}

	return std::any();
}

std::any weec::interpreter::wcExpressionInterpeter::ExecEquality()
{
	auto OpType = PC->Token.Type;
	PC++;

	auto Lh = ChoosePath(PC->Type, Expression);
	auto Rh = ChoosePath(PC->Type, Expression);

	switch (OpType)
	{
	case wcTokenType::PlusOperator:
		return std::any_cast<int>(Lh) + std::any_cast<int>(Rh);

	case wcTokenType::MinusOperator:
		return std::any_cast<int>(Lh) - std::any_cast<int>(Rh);
	}

	return std::any();
}

bool weec::interpreter::wcInterpreter::ExecExpression()
{
	return false;
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

	std::any ExpressionResult;
	while (PC != Input.AST.end())
	{
		switch (PC->Type)
		{
		case Expression:
			ExpressionResult = wcExpressionInterpeter(SymbolTable, Input, PC).Exec();
			break;
		}

		PC++;
	}

	return ExpressionResult;
}

std::any weec::interpreter::wcInterpreter::ExecStatement()
{
	return false;
}
