#include "interpreter.h"

using enum weec::parse::wcParseNodeType;

using namespace std;
using namespace weec::lex;
using namespace weec::parse;
using namespace weec::interpreter;


bool operator==(any& a, any& b)
{
	return true;
}

string weec::interpreter::to_string(wcInterpreterErrorCode Code)
{
	switch (Code)
	{
	case wcInterpreterErrorCode::BadInput:
		return "BadInput";
	case wcInterpreterErrorCode::BadOperation:
		return "BadOperation";
	case wcInterpreterErrorCode::DivByZero:
		return "DivByZero";
	case wcInterpreterErrorCode::InvalidNode:
		return "InvalidNode";
	case wcInterpreterErrorCode::None:
		return "None";
	default:
		return "";
	}
}

any weec::interpreter::wcExpressionInterpreter::EvalNode(wcParseNodeType Type, parse::wcParseNodeType CalledFrom, bool isLValue = false)
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
	case Expression_Call:
		return ExecCall();
	case Expression_Primary:
		return ExecPrimary(isLValue);
	case Expression_Operator:
		return ExecOperator();
	default:
		//error
		break;
	}

	return any();
}

any weec::interpreter::wcExpressionInterpreter::DoOp(lex::wcTokenType Op, any a, any b)
{
	auto FindA = SymTab.ImplTypes.GetByInternal((a.type().name()));
	auto FindB = SymTab.ImplTypes.GetByInternal((b.type().name()));
	if (FindA.Name == "" || FindB.Name == "")
		return any();

	if (Op == wcTokenType::AssignOperator)
	{

	}

	if (FindA.Name == "int")
	{
		if (FindB.Name == "int")
			return AnyOperator<int, int>().DoOp(SymTab, Op, a, b);
		else if (FindB.Name == "unsigned int")
			return AnyOperator<int, unsigned int>().DoOp(SymTab, Op, a, b);
		else if (FindB.Name == "float")
			return AnyOperator<int, float>().DoOp(SymTab, Op, a, b);
		else if (FindB.Name == "double")
			return AnyOperator<int, double>().DoOp(SymTab, Op, a, b);
		else if (FindB.Name == "bool")
			return AnyOperator<int, bool>().DoOp(SymTab, Op, a, b);
		//else if (FindB.Name == "string")
		//	return AnyOperator<int, string>().DoOp(SymTab, Op, a, b);
	}
	else if (FindA.Name == "unsigned int")
	{
		if (FindB.Name == "int")
			return AnyOperator<unsigned int, int>().DoOp(SymTab, Op, a, b);
		else if (FindB.Name == "unsigned int")
			return AnyOperator<unsigned int, unsigned int>().DoOp(SymTab, Op, a, b);
		else if (FindB.Name == "float")
			return AnyOperator<unsigned int, float>().DoOp(SymTab, Op, a, b);
		else if (FindB.Name == "double")
			return AnyOperator<unsigned int, double>().DoOp(SymTab, Op, a, b);
		else if (FindB.Name == "bool")
			return AnyOperator<unsigned int, bool>().DoOp(SymTab, Op, a, b);
		//else if (FindB.Name == "string")
		//	return AnyOperator<unsigned int, string>().DoOp(SymTab, Op, a, b);
	}
	else if (FindA.Name == "float")
	{
		if (FindB.Name == "int")
			return AnyOperator<float, int>().DoOp(SymTab, Op, a, b);
		else if (FindB.Name == "unsigned int")
			return AnyOperator<float, unsigned int>().DoOp(SymTab, Op, a, b);
		else if (FindB.Name == "float")
			return AnyOperator<float, float>().DoOp(SymTab, Op, a, b);
		else if (FindB.Name == "double")
			return AnyOperator<float, double>().DoOp(SymTab, Op, a, b);
		else if (FindB.Name == "bool")
			return AnyOperator<float, bool>().DoOp(SymTab, Op, a, b);
		//else if (FindB.Name == "string")
		//	return AnyOperator<float, string>().DoOp(SymTab, Op, a, b);
	}
	else if (FindA.Name == "double")
	{
		if (FindB.Name == "int")
			return AnyOperator<double, int>().DoOp(SymTab, Op, a, b);
		else if (FindB.Name == "unsigned int")
			return AnyOperator<double, unsigned int>().DoOp(SymTab, Op, a, b);
		else if (FindB.Name == "float")
			return AnyOperator<double, float>().DoOp(SymTab, Op, a, b);
		else if (FindB.Name == "double")
			return AnyOperator<double, double>().DoOp(SymTab, Op, a, b);
		else if (FindB.Name == "bool")
			return AnyOperator<double, bool>().DoOp(SymTab, Op, a, b);
		//else if (FindB.Name == "string")
		//	return AnyOperator<double, string>().DoOp(SymTab, Op, a, b);
	}
	else if (FindA.Name == "bool")
	{
		if (FindB.Name == "int")
			return AnyOperator<bool, int>().DoOp(SymTab, Op, a, b);
		else if (FindB.Name == "unsigned int")
			return AnyOperator<bool, unsigned int>().DoOp(SymTab, Op, a, b);
		else if (FindB.Name == "float")
			return AnyOperator<bool, float>().DoOp(SymTab, Op, a, b);
		else if (FindB.Name == "double")
			return AnyOperator<bool, double>().DoOp(SymTab, Op, a, b);
		else if (FindB.Name == "bool")
			return AnyOperator<bool, bool>().DoOp(SymTab, Op, a, b);
		//else if (FindB.Name == "string")
		//	return AnyOperator<bool, string>().DoOp(SymTab, Op, a, b);
	}
	else if (FindA.Name == "string")
	{
		if (FindB.Name == "int")
			return AnyOperator<string, int>().DoOp(SymTab, Op, a, b);
		else if (FindB.Name == "unsigned int")
			return AnyOperator<string, unsigned int>().DoOp(SymTab, Op, a, b);
		else if (FindB.Name == "float")
			return AnyOperator<string, float>().DoOp(SymTab, Op, a, b);
		else if (FindB.Name == "double")
			return AnyOperator<string, double>().DoOp(SymTab, Op, a, b);
		//else if (FindB.Name == "bool")
		//	return AnyOperator<string, bool>().DoOp(SymTab, Op, a, b);
		else if (FindB.Name == "string")
			return AnyOperator<string, string>().DoOp(SymTab, Op, a, b);
	}

	return any();
}

any weec::interpreter::wcExpressionInterpreter::DoOp(lex::wcTokenType Op, any a)
{
	auto FindA = SymTab.ImplTypes.GetByInternal(a.type().name());
	if (FindA.Name == "")
		return any();

	if (FindA.Name == "int")
		return AnyOperatorUnary<int>().DoOp(SymTab, Op, a);
	else if (FindA.Name == "unsigned int")
		return AnyOperatorUnary<unsigned int>().DoOp(SymTab, Op, a);
	else if (FindA.Name == "float")
		return AnyOperatorUnary<float>().DoOp(SymTab, Op, a);
	else if (FindA.Name == "double")
		return AnyOperatorUnary<double>().DoOp(SymTab, Op, a);
	else if (FindA.Name == "bool")
		return AnyOperatorUnary<bool>().DoOp(SymTab, Op, a);
	else if (FindA.Name == "string")
		return AnyOperatorUnary<bool>().DoOp(SymTab, Op, a);
	return any();
}



weec::interpreter::wcExpressionInterpreter::wcExpressionInterpreter(wcInterpreterSymbolTable& _SymTab, wcInterpreterFunctionTable& _FuncTab, parse::wcParseOutput _Input, tree<parse::wcParseNode>::iterator& _PC, any& _EAX)
	: Input(_Input), SymTab(_SymTab), FuncTab(_FuncTab), PC(_PC), EAX(_EAX)
{
}

any weec::interpreter::wcExpressionInterpreter::ExecSubExpression()
{
	PC++;
	return EvalNode(PC->Type, Expression);
}

any weec::interpreter::wcExpressionInterpreter::ExecCall()
{
	auto Ident = PC->Token;
	auto IdentDepth = Input.AST.depth(PC);
	auto ReturnAddress = PC;
	PC++;
	vector<any> ArgumentValues;

	while (Input.AST.depth(PC) > IdentDepth)
		ArgumentValues.push_back(this->Exec());

	//search for call
	if (!FuncTab.Exists(Ident.StringToken.Data))
		return any();	//error, bad func name
	wcInterpreterStackFrame StackFrame(Ident.StringToken.Data, ReturnAddress, ArgumentValues);
	SymTab.StackFrames.push(StackFrame);

	auto FuncSig = FuncTab.Get(Ident.StringToken.Data);
	vector<wcInterpreterIdentPlusValue> PackedArgs;
	for (int t = 0; t < FuncSig.Arguments.size(); ++t)
	{
		auto IdentString = Ident.StringToken.Data + "::" + wcFullIdent(FuncSig.Arguments[t].Ident.StringToken.Data).ShortIdent.to_string();
		SymTab.StackFrames.top().Add(ArgumentValues[t], wcFullIdent(IdentString));

		PackedArgs.push_back(wcInterpreterIdentPlusValue(wcFullIdent(IdentString), ArgumentValues[t]));
	}
	auto t = FuncSig.Invoke(PackedArgs);
	SymTab.StackFrames.pop();
	return t;
}

any weec::interpreter::wcExpressionInterpreter::ExecOperator()
{
	return EvalNode(PC->Type, Expression);
}

any weec::interpreter::wcExpressionInterpreter::Exec()
{
	return EAX = EvalNode(PC->Type, Expression);
}

any weec::interpreter::wcExpressionInterpreter::ExecPrimary(bool isLValue)
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
	case wcTokenType::TrueKeyword:
	case wcTokenType::FalseKeyword:
		return Result->Token.StringToken.Data == "true" ? true : false;
	case wcTokenType::Identifier:
		if (isLValue)
			return Result->Token.StringToken.Data;
		else
			return this->SymTab.Get(Result->Token.StringToken.Data);
	}
	return any();
}

any weec::interpreter::wcExpressionInterpreter::ExecUnary()
{
	auto OpType = PC->Token.Type;
	PC++;

	auto Lh = EvalNode(PC->Type, Expression);
	if (!strcmp(Lh.type().name(), "struct weec::interpreter::wcInterpreterError"))
		return Lh;
	auto Rh = EvalNode(PC->Type, Expression);
	if (!strcmp(Rh.type().name(), "struct weec::interpreter::wcInterpreterError"))
		return Rh;
	return DoOp(OpType, Lh);
}

any weec::interpreter::wcExpressionInterpreter::ExecFactor()
{
	auto OpType = PC->Token.Type;
	PC++;

	auto Lh = EvalNode(PC->Type, Expression);
	if (!strcmp(Lh.type().name(), "struct weec::interpreter::wcInterpreterError"))
		return Lh;

	auto Rh = EvalNode(PC->Type, Expression);
	if (!strcmp(Rh.type().name(), "struct weec::interpreter::wcInterpreterError"))
		return Rh;

	return DoOp(OpType, Lh, Rh);
}

any weec::interpreter::wcExpressionInterpreter::ExecTerm()
{
	auto OpType = PC->Token.Type;
	PC++;

	auto Lh = EvalNode(PC->Type, Expression);
	if (!strcmp(Lh.type().name(), "struct weec::interpreter::wcInterpreterError"))
		return Lh;
	auto Rh = EvalNode(PC->Type, Expression);
	if (!strcmp(Rh.type().name(), "struct weec::interpreter::wcInterpreterError"))
		return Rh;
	return DoOp(OpType, Lh, Rh);
}

any weec::interpreter::wcExpressionInterpreter::ExecComparison()
{
	auto OpType = PC->Token.Type;
	PC++;

	auto Lh = EvalNode(PC->Type, Expression);
	if (!strcmp(Lh.type().name(), "struct weec::interpreter::wcInterpreterError"))
		return Lh;
	auto Rh = EvalNode(PC->Type, Expression);
	if (!strcmp(Rh.type().name(), "struct weec::interpreter::wcInterpreterError"))
		return Rh;

	return DoOp(OpType, Lh, Rh);
}

any weec::interpreter::wcExpressionInterpreter::ExecLogicAnd()
{
	auto OpType = PC->Token.Type;
	PC++;

	auto Lh = EvalNode(PC->Type, Expression);
	if (Lh.type().name() == "struct weec::interpreter::wcInterpreterError")
		return Lh;
	auto Rh = EvalNode(PC->Type, Expression);
	if (Rh.type().name() == "struct weec::interpreter::wcInterpreterError")
		return Rh;

	return DoOp(OpType, Lh, Rh);
}

any weec::interpreter::wcExpressionInterpreter::ExecLogicOr()
{
	auto OpType = PC->Token.Type;
	PC++;

	auto Lh = EvalNode(PC->Type, Expression);
	if (Lh.type().name() == "struct weec::interpreter::wcInterpreterError")
		return Lh;
	auto Rh = EvalNode(PC->Type, Expression);
	if (Rh.type().name() == "struct weec::interpreter::wcInterpreterError")
		return Rh;

	return DoOp(OpType, Lh, Rh);
}

any weec::interpreter::wcExpressionInterpreter::ExecAssignment()
{
	auto OpType = PC->Token.Type;
	PC++;

	auto Lh = EvalNode(PC->Type, Expression, true);
	if (Lh.type().name() == "struct weec::interpreter::wcInterpreterError")
		return Lh;
	auto Rh = EvalNode(PC->Type, Expression);
	if (Rh.type().name() == "struct weec::interpreter::wcInterpreterError")
		return Rh;

	return DoOp(OpType, Lh, Rh);
}

any weec::interpreter::wcExpressionInterpreter::ExecEquality()
{
	auto OpType = PC->Token.Type;
	PC++;

	auto Lh = EvalNode(PC->Type, Expression);
	if (Lh.type().name() == "struct weec::interpreter::wcInterpreterError")
		return Lh;
	auto Rh = EvalNode(PC->Type, Expression);
	if (Rh.type().name() == "struct weec::interpreter::wcInterpreterError")
		return Rh;

	return DoOp(OpType, Lh, Rh);
}

weec::interpreter::wcInterpreter::wcInterpreter(wcParseOutput& _Input)
	: ExpressionInterp(SymbolTable, FunctionTable, _Input, PC, EAX), Input(_Input), FunctionTable(_Input)
{
	Input = _Input;
	PC = Input.AST.begin();

	//wcInterpreterFunctionSignature GlobalFuncSig(Input, this, vector<wcInterpreterArgument>(), "$g::int", PC);

	//push global stack frame
	SymbolTable.StackFrames.push(wcInterpreterStackFrame("$g", PC, vector<any>()));
}

void weec::interpreter::wcInterpreter::Reset()
{
	Halt = false;
	EAX = any();
	Return = any();
}


any weec::interpreter::wcInterpreter::Exec(tree<wcParseNode>::iterator NewPC, vector<wcInterpreterIdentPlusValue> _Arguments)
{

	//Arguments.push(_Arguments);

	PC = NewPC;
	return Exec();
}

any weec::interpreter::wcInterpreter::Exec()
{
	if (Input.Error.Code != wcParserErrorCode::None)
		return wcInterpreterError(wcInterpreterErrorCode::BadInput, *Input.AST.begin());

	any ExpressionResult, ReturnResult;
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
			PC++;
			break;

		default:
			Error.Node = *PC;
			Error.Code = wcInterpreterErrorCode::InvalidNode;
			Halt = true;
			break;
		}

	}

	return Return;
}

any weec::interpreter::wcInterpreter::ExecStatement()
{
	auto Begin = PC;
	PC++;

	any ExpressionResult;
	if (Input.AST.depth(PC) > Input.AST.depth(Begin))
		switch (PC->Type)
		{
		case Expression:
			ExpressionResult = wcExpressionInterpreter(SymbolTable, FunctionTable, Input, PC, EAX).Exec();
			if (strcmp(ExpressionResult.type().name(), "struct weec::interpreter::wcInterpreterError") == 0)
			{
				Error = any_cast<wcInterpreterError>(ExpressionResult);
				Halt = true;
				return ExpressionResult;
			}
			break;

		case ReturnStatement:
			Return = ExecReturn();
			break;

		case IfStatement:
			ExecIf();
			break;

		case WhileStatement:
			ExecWhile();
			break;

		case Declaration:
			ExecDeclaration();
			break;

		default:
			Error.Node = *PC;
			Error.Code = wcInterpreterErrorCode::InvalidNode;
			Halt = true;
			break;
		}

	return Return;
}

any weec::interpreter::wcInterpreter::ExecReturn()
{
	auto Begin = PC;
	PC++;

	while (Input.AST.depth(PC) > Input.AST.depth(Begin) && (PC != Input.AST.end() && !Halt))
		switch (PC->Type)
		{
		case Expression:
			Return = wcExpressionInterpreter(SymbolTable, FunctionTable, Input, PC, EAX).Exec();
			break;

		default:
			Error.Node = *PC;
			Error.Code = wcInterpreterErrorCode::InvalidNode;
			Halt = true;
			break;
		}

	Halt = true;
	return Return;
}

any weec::interpreter::wcInterpreter::ExecBlock()
{
	auto Begin = PC;
	PC++;

	while (Input.AST.depth(PC) > Input.AST.depth(Begin) && (PC != Input.AST.end() && !Halt))
		switch (PC->Type)
		{
		case Statement:
			ExecStatement();
			break;

		case Block:
			ExecBlock();
			break;

		default:
			Error.Node = *PC;
			Error.Code = wcInterpreterErrorCode::InvalidNode;
			Halt = true;
			break;
		}

	return any();
}

any weec::interpreter::wcInterpreter::SkipBlock()
{
	auto Begin = PC;
	//PC = Input.AST.next_sibling(PC);
	PC++;

	while (Input.AST.depth(PC) > Input.AST.depth(Begin) && (PC != Input.AST.end() && !Halt))
		PC++;

	return any();
}

any weec::interpreter::wcInterpreter::ExecIf()
{
	auto Begin = PC;
	PC++;

	wcToken TypeToken, IdentToken;
	any ExpressionResult;
	bool ExprTrue = false;
	while (Input.AST.depth(PC) > Input.AST.depth(Begin) && (PC != Input.AST.end() && PC.node != nullptr && !Halt))
	{
		switch (PC->Type)
		{
		case Expression:
			ExpressionResult = wcExpressionInterpreter(SymbolTable, FunctionTable, Input, PC, EAX).Exec();
			ExprTrue = any_cast<bool>(ExpressionResult);
			break;

		case If_TrueBlock:
			//PC++;
			if (ExprTrue)
				ExecBlock();
			else
				SkipBlock();
			break;

		case If_ElseBlock:
			//PC++;
			if (ExprTrue)
				SkipBlock();
			else
				ExecBlock();
			break;

		default:
			Error.Node = *PC;
			Error.Code = wcInterpreterErrorCode::InvalidNode;
			Halt = true;
			break;
		}
	}

	return ExprTrue;
}

any weec::interpreter::wcInterpreter::ExecWhile()
{
	auto Begin = PC;
	PC++;

	tree<wcParseNode>::iterator ExprPC = PC;
	tree<wcParseNode>::iterator WhileBlockPC = PC;
	any ExpressionResult;
	bool ExprTrue = false;
	while (Input.AST.depth(PC) > Input.AST.depth(Begin) && (PC != Input.AST.end() && PC.node != nullptr && !Halt))
	{
		switch (PC->Type)
		{
		case Expression:
			ExprPC = PC;
			ExpressionResult = wcExpressionInterpreter(SymbolTable, FunctionTable, Input, PC, EAX).Exec();
			ExprTrue = any_cast<bool>(ExpressionResult);
			break;

		case Block:
			WhileBlockPC = PC;
			if (ExprTrue)
				ExecBlock();
			else
				SkipBlock();
			PC = ExprPC;
			break;

		case Statement:
			WhileBlockPC = PC;
			if (ExprTrue)
				ExecStatement();
			PC = ExprPC;
			break;

		default:
			Error.Node = *PC;
			Error.Code = wcInterpreterErrorCode::InvalidNode;
			Halt = true;
			break;
		}

		if (!ExprTrue)
			return false;

	}

	return ExprTrue;
}

any weec::interpreter::wcInterpreter::ExecDeclaration()
{
	auto Begin = PC;
	PC++;

	wcToken TypeToken, IdentToken, ArgIdent, ArgType;
	bool isFunc = false;
	unsigned int DeclarationArgDepth;
	tree<wcParseNode>::iterator DecBlockPC = PC;
	vector<wcInterpreterArgument> Arguments;
	string SymbolName = "";
	while (Input.AST.depth(PC) > Input.AST.depth(Begin) && (PC != Input.AST.end() && PC.node != nullptr && !Halt))
	{
		switch (PC->Type)
		{
		case Declaration_Type:
			TypeToken = PC->Token;
			PC++;
			break;

		case Declaration_Ident:
			IdentToken = PC->Token;
			SymbolName = IdentToken.StringToken.Data;
			PC++;
			break;

		case DeclarationArguments:
			isFunc = true;

			DeclarationArgDepth = Input.AST.depth(PC);
			PC++;

			while (Input.AST.depth(PC) > DeclarationArgDepth)
			{
				PC++;
				ArgType = PC->Token;
				PC++;
				ArgIdent = PC->Token;
				PC++;
				Arguments.push_back(wcInterpreterArgument(ArgType, ArgIdent));
			}

			SymbolName += "";
			break;

		case Expression:
			wcExpressionInterpreter(SymbolTable, FunctionTable, Input, PC, EAX).Exec();
			break;

		case Block:
			DecBlockPC = PC;
			SkipBlock();
			break;

		default:
			Error.Node = *PC;
			Error.Code = wcInterpreterErrorCode::InvalidNode;
			Halt = true;
			break;
		}
	}



	//add to symbol table
	if (!isFunc)
	{
		if (TypeToken.StringToken.Data == "int")
			SymbolTable.Add(any_cast<int>(EAX.has_value() ? EAX : 0), IdentToken.StringToken.Data);
		else if (TypeToken.StringToken.Data == "float")
			SymbolTable.Add(any_cast<float>(EAX.has_value() ? EAX : 0), IdentToken.StringToken.Data);
		else if (TypeToken.StringToken.Data == "unsigned int")
			SymbolTable.Add(any_cast<unsigned int>(EAX.has_value() ? EAX : 0), IdentToken.StringToken.Data);
		else if (TypeToken.StringToken.Data == "double")
			SymbolTable.Add(any_cast<double>(EAX.has_value() ? EAX : 0), IdentToken.StringToken.Data);
		else if (TypeToken.StringToken.Data == "bool")
			SymbolTable.Add(any_cast<bool>(EAX.has_value() ? EAX : 0), IdentToken.StringToken.Data);
	}
	else
	{
		//register args
		for (auto Arg : Arguments)
			if (TypeToken.StringToken.Data == "int")
				SymbolTable.Add(any_cast<int>(EAX.has_value() ? EAX : 0), IdentToken.StringToken.Data + "::" + wcFullIdent(Arg.Ident.StringToken.Data).ShortIdent.to_string());
			else if (TypeToken.StringToken.Data == "float")
				SymbolTable.Add(any_cast<float>(EAX.has_value() ? EAX : 0), IdentToken.StringToken.Data + "::" + wcFullIdent(Arg.Ident.StringToken.Data).ShortIdent.to_string());
			else if (TypeToken.StringToken.Data == "unsigned int")
				SymbolTable.Add(any_cast<unsigned int>(EAX.has_value() ? EAX : 0), IdentToken.StringToken.Data + "::" + wcFullIdent(Arg.Ident.StringToken.Data).ShortIdent.to_string());
			else if (TypeToken.StringToken.Data == "double")
				SymbolTable.Add(any_cast<double>(EAX.has_value() ? EAX : 0), IdentToken.StringToken.Data + "::" + wcFullIdent(Arg.Ident.StringToken.Data).ShortIdent.to_string());
			else if (TypeToken.StringToken.Data == "bool")
				SymbolTable.Add(any_cast<bool>(EAX.has_value() ? EAX : 0), IdentToken.StringToken.Data + "::" + wcFullIdent(Arg.Ident.StringToken.Data).ShortIdent.to_string());


		FunctionTable.Add(wcInterpreterFunctionSignature(Input, *this, Arguments, TypeToken.StringToken.Data, DecBlockPC), IdentToken.StringToken.Data);
	}

	return EAX;
}


weec::interpreter::wcInterpreterSymbolTable::wcInterpreterSymbolTable()
{

}

bool weec::interpreter::wcInterpreterSymbolTable::Add(any Value, wcFullIdent FullIdent)
{
	StackFrames.top().Add(Value, FullIdent.to_string());
	return true;
}

any weec::interpreter::wcInterpreterSymbolTable::Get(wcFullIdent FullIdent) const
{
	return StackFrames.top().Container.find(FullIdent.to_string())->second;
}

void weec::interpreter::wcInterpreterSymbolTable::Set(wcFullIdent FullIdent, any Value)
{
	StackFrames.top().Container.find(FullIdent.to_string())->second = Value;
}

weec::interpreter::ImplementationTypes::ImplementationTypes()
{
	Add("int", type_index(typeid(int)));
	Add("unsigned int", type_index(typeid(unsigned int)));
	Add("float", type_index(typeid(float)));
	Add("double", type_index(typeid(double)));
	Add("bool", type_index(typeid(bool)));
	Add("string", type_index(typeid(string)));
}

void weec::interpreter::ImplementationTypes::Add(string EasyName, type_index Type)
{
	EasyTypeNames.insert(make_pair(EasyName, ImplementationType(EasyName, Type)));
	InternalTypeNames.insert(make_pair(Type.name(), ImplementationType(EasyName, Type)));
}

ImplementationType weec::interpreter::ImplementationTypes::Get(string EasyName)
{
	if (EasyTypeNames.find(EasyName) == EasyTypeNames.end())
		return ImplementationType();
	return EasyTypeNames[EasyName];
}

ImplementationType weec::interpreter::ImplementationTypes::GetByInternal(string InternalName)
{
	if (InternalTypeNames.find(InternalName) == InternalTypeNames.end())
		return ImplementationType();
	return InternalTypeNames[InternalName];
}

weec::interpreter::wcInterpreterFunctionSignature::wcInterpreterFunctionSignature(wcParseOutput& _Input, wcInterpreter& _Interpreter, vector<wcInterpreterArgument> _Arguments, string _ReturnDataType, tree<wcParseNode>::iterator _Block)
	: Input(_Input), Interpreter(_Interpreter), Block(_Block)
{
	for (auto Arg : _Arguments)
		Arguments.push_back(Arg);
	ReturnDataType = _ReturnDataType;
}

weec::interpreter::wcInterpreterFunctionTable::wcInterpreterFunctionTable(wcParseOutput& _Input)
	: Input(_Input)
{
}

bool weec::interpreter::wcInterpreterFunctionTable::Add(wcInterpreterFunctionSignature Sig, string FullIdent)
{
	if (Exists(FullIdent))
		return false;

	Container.insert(make_pair(FullIdent, Sig));

	return true;
}

bool weec::interpreter::wcInterpreterFunctionTable::Exists(string FullIdent)
{
	return Container.find(FullIdent) != Container.end();
}

wcInterpreterFunctionSignature weec::interpreter::wcInterpreterFunctionTable::Get(string FullIdent)
{
	return Container.find(FullIdent)->second;
}

any weec::interpreter::wcInterpreterFunctionSignature::Invoke()
{
	return Interpreter.Exec(Block, vector<wcInterpreterIdentPlusValue>());
}

any weec::interpreter::wcInterpreterFunctionSignature::Invoke(vector<wcInterpreterIdentPlusValue> Arguments)
{
	return Interpreter.Exec(Block, Arguments);
}

bool weec::interpreter::wcInterpreterStackFrame::Add(std::any Value, parse::wcFullIdent FullIdent)
{
	Container.insert(make_pair(FullIdent.to_string(), Value));
	return true;
}

std::any weec::interpreter::wcInterpreterStackFrame::Get(parse::wcFullIdent FullIdent) const
{
	return Container.find(FullIdent.to_string())->second;
}

void weec::interpreter::wcInterpreterStackFrame::Set(parse::wcFullIdent FullIdent, std::any Value)
{
	Container.find(FullIdent.to_string())->second = Value;
}