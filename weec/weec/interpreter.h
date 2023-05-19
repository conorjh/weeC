#ifndef WC_INTERPRETER
#define WC_INTERPRETER
#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif
#include <any>
#include <string>
#include <vector>
#include <unordered_map>
#include "tree.hh"
#include "lex.h"
#include "parse.h"
#include <typeindex>
#include <functional>
#include <stack>

namespace weec
{
	namespace interpreter
	{
		class ImplementationType
		{
		public:
			ImplementationType()
				: TypeIndex(typeid(void)) {

			}

			ImplementationType(std::string _Name, std::type_index _TypeIndex)
				: TypeIndex(_TypeIndex) {
				Name = _Name;
				InternalName = _TypeIndex.name();
			}

			std::string Name, InternalName;
			std::type_index TypeIndex;
		};
		class wcInterpreter;

		struct wcInterpreterArgument
		{
			wcInterpreterArgument(lex::wcToken _Type, lex::wcToken _Ident)
			{
				Type = _Type;
				Ident = _Ident;
			}
			lex::wcToken Type, Ident;
		};

		struct wcInterpreterIdentPlusValue
		{
			wcInterpreterIdentPlusValue(parse::wcFullIdentifier _Arg, std::any _Value) { Arg = _Arg; Value = _Value; }
			parse::wcFullIdentifier Arg;
			std::any Value;
		};

		struct wcInterpreterFunctionSignature
		{
			parse::wcParseOutput& Input;
			tree<parse::wcParseNode>::iterator Block;
			wcInterpreter& Interpreter;
			std::vector<wcInterpreterArgument> Arguments;
			std::string ReturnDataType;

		public:
			wcInterpreterFunctionSignature(parse::wcParseOutput&, wcInterpreter& Interpreter, std::vector<wcInterpreterArgument> Arguments, std::string ReturnDataType, tree<parse::wcParseNode>::iterator Block);

			std::any Invoke(), Invoke(std::vector<wcInterpreterIdentPlusValue> Arguments);
		};

		class wcInterpreterFunctionTable
		{
			parse::wcParseOutput& Input;
			std::unordered_map<std::string, wcInterpreterFunctionSignature> Container;
		public:

			wcInterpreterFunctionTable(parse::wcParseOutput&);

			bool Add(wcInterpreterFunctionSignature, std::string FullIdent),
				Exists(std::string FullIdent);

			wcInterpreterFunctionSignature Get(std::string FullIdent);
		};


		class ImplementationTypes
		{
			std::unordered_map<std::string, ImplementationType> EasyTypeNames;
			std::unordered_map<std::string, ImplementationType> InternalTypeNames;

		public:
			ImplementationTypes();
			ImplementationType operator[](std::string a) { return Get(a); }
			void Add(std::string EasyName, std::type_index Type);
			ImplementationType Get(std::string EasyName), GetByInternal(std::string InternalName);
		};

		struct wcInterpreterStackFrame
		{
			wcInterpreterStackFrame(std::string _FrameName, tree<parse::wcParseNode>::iterator _ReturnAddress, std::vector<std::any> _Arguments)
			{
				FrameName = _FrameName;
				ReturnAddress = _ReturnAddress;
				Arguments = _Arguments;
			}

			bool Add(std::any, parse::wcFullIdentifier FullIdent);
			std::any Get(parse::wcFullIdentifier FullIdent) const;
			void Set(parse::wcFullIdentifier FullIdent, std::any);

			std::string FrameName;
			tree<parse::wcParseNode>::iterator ReturnAddress;
			std::vector<std::any> Arguments;
			std::unordered_map<std::string, std::any> Container;
		};

		class wcInterpreterSymbolTable 
		{
			std::unordered_map<std::string, std::any> Container;
		public:
			std::stack<wcInterpreterStackFrame> StackFrames;

			wcInterpreterSymbolTable();

			bool Add(std::any, parse::wcFullIdentifier FullIdent),
				Exists(parse::wcFullIdentifier FullIdent) const;

			std::any Get(parse::wcFullIdentifier FullIdent) const;
			void Set(parse::wcFullIdentifier FullIdent, std::any);

			ImplementationTypes ImplTypes;
		};

		template<typename T1, typename T2>
		class AnyOperator
		{
		public:
			std::any DoOp(wcInterpreterSymbolTable&, lex::wcTokenType Op, std::any a, std::any b);
		};

		template<typename T1>
		class AnyOperatorUnary
		{
		public:
			std::any DoOp(wcInterpreterSymbolTable&, lex::wcTokenType Op, std::any a);
		};


		class wcExpressionInterpreter
		{
			parse::wcParseOutput& Input;
			tree<parse::wcParseNode>::iterator& PC;
			wcInterpreterSymbolTable& SymTab;
			wcInterpreterFunctionTable& FuncTab;
			std::any& EAX;

			std::any EvalNode(parse::wcParseNodeType Type, parse::wcParseNodeType CalledFrom, bool);

			std::any DoOp(lex::wcTokenType Op, std::any a, std::any b);
			std::any DoOp(lex::wcTokenType Op, std::any a);


		public:
			wcExpressionInterpreter(wcInterpreterSymbolTable& SymTab, wcInterpreterFunctionTable& FuncTab,parse::wcParseOutput Input, tree<parse::wcParseNode>::iterator& PC, std::any& EAX);

			std::any ExecSubExpression(),
				ExecEquality(), ExecAssignment(), ExecLogicOr(),
				ExecLogicAnd(), ExecComparison(),
				ExecTerm(), ExecFactor(),
				ExecUnary(), ExecCall(), ExecPrimary(bool),
				ExecOperator();

			std::any Exec();
		};

		enum class wcInterpreterErrorCode
		{
			None, BadInput, InvalidNode, DivByZero, BadOperation
		};
		std::string to_string(wcInterpreterErrorCode Code);

		struct wcInterpreterError
		{
			wcInterpreterError()
			{
				Code = wcInterpreterErrorCode::None;
			}
			wcInterpreterError(wcInterpreterErrorCode _Code, parse::wcParseNode _Node)
			{
				Code = _Code;
				Node = _Node;
			}
			wcInterpreterError(wcInterpreterErrorCode _Code)
			{
				Code = _Code;
			}

			wcInterpreterErrorCode Code;
			parse::wcParseNode Node;
		};

		class wcInterpreter
		{
			parse::wcParseOutput& Input;
			tree<parse::wcParseNode>::iterator PC;
			wcInterpreterSymbolTable SymbolTable;
			wcInterpreterFunctionTable FunctionTable;

		public:
			wcExpressionInterpreter ExpressionInterp;
			wcInterpreter(parse::wcParseOutput& Input);

			void Reset();
			std::any Exec(), 
				Exec(tree<parse::wcParseNode>::iterator NewPC), 
				Exec(tree<parse::wcParseNode>::iterator NewPC, std::vector<wcInterpreterIdentPlusValue> Arguments),
				
				ExecBlock(), SkipBlock(), ExecStatement(), ExecIf(), ExecWhile(), ExecReturn(), ExecDeclaration();

			wcInterpreterError Error;
			bool Halt;
			std::any EAX, Return;
		};

		template <>
		inline std::any AnyOperator<std::string, int>::DoOp(wcInterpreterSymbolTable& SymTab, lex::wcTokenType Op, std::any a, std::any b)
		{
			switch (Op)
			{
			case lex::wcTokenType::AssignOperator:
				SymTab.Set(std::any_cast<std::string>(a), std::any_cast<int>(b));
				return std::any_cast<int>(b);

			default:
				return  wcInterpreterError(wcInterpreterErrorCode::BadOperation);	//err
			}
		}

		template <>
		inline std::any AnyOperator<std::string, unsigned int>::DoOp(wcInterpreterSymbolTable& SymTab, lex::wcTokenType Op, std::any a, std::any b)
		{
			switch (Op)
			{
			case lex::wcTokenType::AssignOperator:
				SymTab.Set(std::any_cast<std::string>(a), std::any_cast<unsigned int>(b));
				return std::any_cast<unsigned int>(b);

			default:
				return  wcInterpreterError(wcInterpreterErrorCode::BadOperation);	//err
			}
		}

		template <>
		inline std::any AnyOperator<std::string, float>::DoOp(wcInterpreterSymbolTable& SymTab, lex::wcTokenType Op, std::any a, std::any b)
		{
			switch (Op)
			{
			case lex::wcTokenType::AssignOperator:
				SymTab.Set(std::any_cast<std::string>(a), std::any_cast<float>(b));
				return std::any_cast<float>(b);

			default:
				return  wcInterpreterError(wcInterpreterErrorCode::BadOperation);	//err
			}
		}

		template <>
		inline std::any AnyOperator<std::string, double>::DoOp(wcInterpreterSymbolTable& SymTab, lex::wcTokenType Op, std::any a, std::any b)
		{
			switch (Op)
			{
			case lex::wcTokenType::AssignOperator:
				SymTab.Set(std::any_cast<std::string>(a), std::any_cast<double>(b));
				return std::any_cast<double>(b);

			default:
				return  wcInterpreterError(wcInterpreterErrorCode::BadOperation);	//err
			}
		}

		template <>
		inline std::any AnyOperator<std::string, std::string>::DoOp(wcInterpreterSymbolTable& SymTab, lex::wcTokenType Op, std::any a, std::any b)
		{
			switch (Op)
			{
			case lex::wcTokenType::AssignOperator:
				SymTab.Set(std::any_cast<std::string>(a), std::any_cast<std::string>(b));
				return std::any_cast<std::string>(b);

			default:
				return  wcInterpreterError(wcInterpreterErrorCode::BadOperation);	//err
			}
		}

		template<typename T1, typename T2>
		inline std::any AnyOperator<T1, T2>::DoOp(wcInterpreterSymbolTable& SymTab, lex::wcTokenType Op, std::any a, std::any b)
		{
			//manually handle divide
			if (Op == lex::wcTokenType::DivideOperator)
			{
				if (!strcmp(std::type_index(typeid(T1)).name(), "int"))
				{
					if (!strcmp(std::type_index(typeid(T2)).name(), "int"))
						if (std::any_cast<int>(b) != 0)
							return std::any_cast<int>(a) / std::any_cast<int>(b);
						else return wcInterpreterError(wcInterpreterErrorCode::DivByZero);

					else if (!strcmp(std::type_index(typeid(T2)).name(), "unsigned int"))
						if (std::any_cast<unsigned int>(b) != 0)
							return std::any_cast<int>(a) / std::any_cast<unsigned int>(b);
						else return wcInterpreterError(wcInterpreterErrorCode::DivByZero);

					else if (!strcmp(std::type_index(typeid(T2)).name(), "double"))
						if (std::any_cast<double>(b) != 0)
							return std::any_cast<int>(a) / std::any_cast<double>(b);
						else return wcInterpreterError(wcInterpreterErrorCode::DivByZero);

					else if (!strcmp(std::type_index(typeid(T2)).name(), "float"))
						if (std::any_cast<float>(b) != 0)
							return std::any_cast<int>(a) / std::any_cast<float>(b);
						else return wcInterpreterError(wcInterpreterErrorCode::DivByZero);

					else if (!strcmp(std::type_index(typeid(T2)).name(), "bool"))
						if (std::any_cast<bool>(b) != 0)
							return std::any_cast<int>(a) / std::any_cast<bool>(b);
						else return wcInterpreterError(wcInterpreterErrorCode::DivByZero);

				}
				else if (!strcmp(std::type_index(typeid(T1)).name(), "unsigned int"))
				{
					if (!strcmp(std::type_index(typeid(T2)).name(), "int"))
						if (std::any_cast<int>(b) != 0)
							return std::any_cast<unsigned int>(a) / std::any_cast<int>(b);
						else return wcInterpreterError(wcInterpreterErrorCode::DivByZero);

					else if (!strcmp(std::type_index(typeid(T2)).name(), "unsigned int"))
						if (std::any_cast<unsigned int>(b) != 0)
							return std::any_cast<unsigned int>(a) / std::any_cast<unsigned int>(b);
						else return wcInterpreterError(wcInterpreterErrorCode::DivByZero);

					else if (!strcmp(std::type_index(typeid(T2)).name(), "double"))
						if (std::any_cast<double>(b) != 0)
							return std::any_cast<unsigned int>(a) / std::any_cast<double>(b);
						else return wcInterpreterError(wcInterpreterErrorCode::DivByZero);
					else if (!strcmp(std::type_index(typeid(T2)).name(), "float"))
						if (std::any_cast<float>(b) != 0)
							return std::any_cast<unsigned int>(a) / std::any_cast<float>(b);
						else return wcInterpreterError(wcInterpreterErrorCode::DivByZero);
					else if (!strcmp(std::type_index(typeid(T2)).name(), "bool"))
						if (std::any_cast<bool>(b) != 0)
							return std::any_cast<unsigned int>(a) / std::any_cast<bool>(b);
						else return wcInterpreterError(wcInterpreterErrorCode::DivByZero);
				}
				else if (!strcmp(std::type_index(typeid(T1)).name(), "double"))
				{
					if (!strcmp(std::type_index(typeid(T2)).name(), "int"))
						if (std::any_cast<int>(b) != 0)
							return double(std::any_cast<double>(a) / std::any_cast<int>(b));
						else return wcInterpreterError(wcInterpreterErrorCode::DivByZero);
					else if (!strcmp(std::type_index(typeid(T2)).name(), "unsigned int"))
						if (std::any_cast<unsigned int>(b) != 0)
							return double(std::any_cast<double>(a) / std::any_cast<unsigned int>(b));
						else return wcInterpreterError(wcInterpreterErrorCode::DivByZero);
					else if (!strcmp(std::type_index(typeid(T2)).name(), "double"))
						if (std::any_cast<double>(b) != 0)
							return double(std::any_cast<double>(a) / std::any_cast<double>(b));
						else return wcInterpreterError(wcInterpreterErrorCode::DivByZero);
					else if (!strcmp(std::type_index(typeid(T2)).name(), "float"))
						if (std::any_cast<float>(b) != 0)
							return double(std::any_cast<double>(a) / std::any_cast<float>(b));
						else return wcInterpreterError(wcInterpreterErrorCode::DivByZero);
					else if (!strcmp(std::type_index(typeid(T2)).name(), "bool"))
						if (std::any_cast<bool>(b) != 0)
							return double(std::any_cast<double>(a) / std::any_cast<bool>(b));
						else return wcInterpreterError(wcInterpreterErrorCode::DivByZero);
				}
				else if (!strcmp(std::type_index(typeid(T1)).name(), "float"))
				{
					if (!strcmp(std::type_index(typeid(T2)).name(), "int"))
						if (std::any_cast<int>(b) != 0)
							return float(std::any_cast<float>(a) / std::any_cast<int>(b));
						else return wcInterpreterError(wcInterpreterErrorCode::DivByZero);
					else if (!strcmp(std::type_index(typeid(T2)).name(), "unsigned int"))
						if (std::any_cast<unsigned int>(b) != 0)
							return float(std::any_cast<float>(a) / std::any_cast<unsigned int>(b));
						else return wcInterpreterError(wcInterpreterErrorCode::DivByZero);
					else if (!strcmp(std::type_index(typeid(T2)).name(), "double"))
						if (std::any_cast<double>(b) != 0)
							return float(std::any_cast<float>(a) / std::any_cast<double>(b));
						else return wcInterpreterError(wcInterpreterErrorCode::DivByZero);
					else if (!strcmp(std::type_index(typeid(T2)).name(), "float"))
						if (std::any_cast<float>(b) != 0)
							return float(std::any_cast<float>(a) / std::any_cast<float>(b));
						else return wcInterpreterError(wcInterpreterErrorCode::DivByZero);
					else if (!strcmp(std::type_index(typeid(T2)).name(), "bool"))
						if (std::any_cast<bool>(b) != 0)
							return float(std::any_cast<float>(a) / std::any_cast<bool>(b));
						else return wcInterpreterError(wcInterpreterErrorCode::DivByZero);
				}
				else if (!strcmp(std::type_index(typeid(T1)).name(), "string"))
				{
					if (!strcmp(std::type_index(typeid(T2)).name(), "int"))
						if (std::any_cast<int>(b) != 0)
							return std::any_cast<bool>(a) / std::any_cast<int>(b);
						else return wcInterpreterError(wcInterpreterErrorCode::DivByZero);
					else if (!strcmp(std::type_index(typeid(T2)).name(), "unsigned int"))
						if (std::any_cast<unsigned int>(b) != 0)
							return std::any_cast<bool>(a) / std::any_cast<unsigned int>(b);
						else return wcInterpreterError(wcInterpreterErrorCode::DivByZero);
					else if (!strcmp(std::type_index(typeid(T2)).name(), "double"))
						if (std::any_cast<double>(b) != 0)
							return std::any_cast<bool>(a) / std::any_cast<double>(b);
						else return wcInterpreterError(wcInterpreterErrorCode::DivByZero);
					else if (!strcmp(std::type_index(typeid(T2)).name(), "float"))
						if (std::any_cast<float>(b) != 0)
							return std::any_cast<bool>(a) / std::any_cast<float>(b);
						else return wcInterpreterError(wcInterpreterErrorCode::DivByZero);
					else if (!strcmp(std::type_index(typeid(T2)).name(), "bool"))
						if (std::any_cast<bool>(b) != 0)
							return std::any_cast<bool>(a) / std::any_cast<bool>(b);
						else return wcInterpreterError(wcInterpreterErrorCode::DivByZero);
				}
			}
			
			switch (Op)
			{
			case lex::wcTokenType::MultiplyOperator:
				return std::any_cast<T1>(a) * std::any_cast<T2>(b);
			case lex::wcTokenType::PlusOperator:
				return std::any_cast<T1>(a) + std::any_cast<T2>(b);
			case lex::wcTokenType::MinusOperator:
				return std::any_cast<T1>(a) - std::any_cast<T2>(b);
			case lex::wcTokenType::GreaterEqualOperator:
				return std::any_cast<T1>(a) >= std::any_cast<T2>(b);
			case lex::wcTokenType::GreaterOperator:
				return std::any_cast<T1>(a) > std::any_cast<T2>(b);
			case lex::wcTokenType::LessEqualOperator:
				return std::any_cast<T1>(a) <= std::any_cast<T2>(b);
			case lex::wcTokenType::LessOperator:
				return std::any_cast<T1>(a) < std::any_cast<T2>(b);
			case lex::wcTokenType::LogAndOperator:
				return std::any_cast<T1>(a) && std::any_cast<T2>(b);
			case lex::wcTokenType::LogOrOperator:
				return std::any_cast<T1>(a) || std::any_cast<T2>(b);
			case lex::wcTokenType::EqualOperator:
				return std::any_cast<T1>(a) == std::any_cast<T2>(b);
			case lex::wcTokenType::NotEqualOperator:
				return std::any_cast<T1>(a) != std::any_cast<T2>(b);

			case lex::wcTokenType::AssignOperator:
				SymTab.Set(std::any_cast<std::string>(a), std::any_cast<T1>(b));
				return std::any_cast<T1>(b);
			}

			return  wcInterpreterError(wcInterpreterErrorCode::BadOperation);	//err
		}

		template<typename T1>
		inline std::any AnyOperatorUnary<T1>::DoOp(wcInterpreterSymbolTable& SymTab, lex::wcTokenType Op, std::any a)
		{
			switch (Op)
			{
			case lex::wcTokenType::LogNotOperator:
				return !std::any_cast<T1>(a);
			case lex::wcTokenType::MinusOperator:
				return -std::any_cast<T1>(a);
			case lex::wcTokenType::PlusOperator:
				return +std::any_cast<T1>(a);
			}

			return  wcInterpreterError(wcInterpreterErrorCode::BadOperation);	//err
		}

	}

}


#endif
