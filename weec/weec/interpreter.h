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

namespace weec
{
	namespace interpreter
	{
		class wcInterpreterSymbolTable
		{
			std::unordered_map<std::string, std::any> Container;

		public:
			wcInterpreterSymbolTable();

			bool Add(std::any, std::string FullIdent),

				Exists(std::string FullIdent) const;

			std::any Get(std::string FullIdent) const;
			void Set(std::string FullIdent, std::any);
		};

		class ImplementationType
		{
		public:
			ImplementationType(std::string _Name, std::type_index _TypeIndex)
				: TypeIndex(_TypeIndex) {
				Name = _Name;
			}

			std::string Name;
			std::type_index TypeIndex;
		};

		template<typename T1, typename T2>
		class AnyOperator
		{
		public:
			std::any DoOp(lex::wcTokenType Op, std::any a, std::any b);
		};

		template<typename T1>
		class AnyOperatorUnary
		{
		public:
			std::any DoOp(lex::wcTokenType Op, std::any a);
		};

		class wcExpressionInterpeter
		{
			int Value;

			parse::wcParseOutput& Input;
			tree<parse::wcParseNode>::iterator& PC;
			wcInterpreterSymbolTable& SymTab;
			std::any& EAX;

			std::unordered_map<std::string, ImplementationType> ImplementationTypes;
			void SetupImplementationTypeNames();

			std::any EvalNode(parse::wcParseNodeType Type, parse::wcParseNodeType CalledFrom);

			std::any DoOp(lex::wcTokenType Op, std::any a, std::any b);
			std::any DoOp(lex::wcTokenType Op, std::any a);

		public:
			wcExpressionInterpeter(wcInterpreterSymbolTable& SymTab, parse::wcParseOutput Input, tree<parse::wcParseNode>::iterator& PC, std::any& EAX);

			std::any ExecSubExpression(),
				ExecEquality(), ExecAssignment(), ExecLogicOr(),
				ExecLogicAnd(), ExecComparison(),
				ExecTerm(), ExecFactor(),
				ExecUnary(), ExecPrimary(),
				ExecOperator();

			std::any Exec();
		};

		enum class wcInterpreterErrorCode
		{
			None, BadInput, InvalidNode, DivByZero
		};

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

			wcInterpreterErrorCode Code;
			parse::wcParseNode Node;
		};


		class wcInterpreter
		{
			parse::wcParseOutput Input;
			tree<parse::wcParseNode>::iterator PC;
			wcInterpreterSymbolTable SymbolTable;

		public:
			wcExpressionInterpeter ExpressionInterp;
			wcInterpreter(parse::wcParseOutput Input);

			void Reset();
			std::any Exec(), ExecBlock(), SkipBlock(), ExecStatement(), ExecIf(), ExecReturn(), ExecDeclaration();

			wcInterpreterError Error;
			bool Halt;
			std::any EAX, Return;
		};


		template<typename T1, typename T2>
		inline std::any AnyOperator<T1, T2>::DoOp(lex::wcTokenType Op, std::any a, std::any b)
		{
			//manually handle divide
			if (Op == lex::wcTokenType::DivideOperator)
				if (!strcmp(std::type_index(typeid(T1)).name(), "int"))
				{
					if (!strcmp(std::type_index(typeid(T2)).name(), "int"))
						return std::any_cast<int>(b) != 0 ? std::any_cast<int>(a) / std::any_cast<int>(b) : std::any();
					else if (!strcmp(std::type_index(typeid(T2)).name(), "unsigned int"))
						return std::any_cast<unsigned int>(b) != 0 ? std::any_cast<int>(a) / std::any_cast<unsigned int>(b) : std::any();
					else if (!strcmp(std::type_index(typeid(T2)).name(), "double"))
						return std::any_cast<double>(b) != 0 ? std::any_cast<int>(a) / std::any_cast<double>(b) : std::any();
					else if (!strcmp(std::type_index(typeid(T2)).name(), "float"))
						return std::any_cast<float>(b) != 0 ? std::any_cast<int>(a) / std::any_cast<float>(b) : std::any();
					else if (!strcmp(std::type_index(typeid(T2)).name(), "bool"))
						return std::any_cast<bool>(b) != 0 ? std::any_cast<int>(a) / std::any_cast<bool>(b) : std::any();
				}
				else if (!strcmp(std::type_index(typeid(T1)).name(), "unsigned int"))
				{
					if (!strcmp(std::type_index(typeid(T2)).name(), "int"))
						return std::any_cast<int>(b) != 0 ? std::any_cast<unsigned int>(a) / std::any_cast<int>(b) : std::any();
					else if (!strcmp(std::type_index(typeid(T2)).name(), "unsigned int"))
						return std::any_cast<unsigned int>(b) != 0 ? std::any_cast<unsigned int>(a) / std::any_cast<unsigned int>(b) : std::any();
					else if (!strcmp(std::type_index(typeid(T2)).name(), "double"))
						return std::any_cast<double>(b) != 0 ? std::any_cast<unsigned int>(a) / std::any_cast<double>(b) : std::any();
					else if (!strcmp(std::type_index(typeid(T2)).name(), "float"))
						return std::any_cast<float>(b) != 0 ? std::any_cast<unsigned int>(a) / std::any_cast<float>(b) : std::any();
					else if (!strcmp(std::type_index(typeid(T2)).name(), "bool"))
						return std::any_cast<bool>(b) != 0 ? std::any_cast<unsigned int>(a) / std::any_cast<bool>(b) : std::any();
				}
				else if (!strcmp(std::type_index(typeid(T1)).name(), "double"))
				{
					if (!strcmp(std::type_index(typeid(T2)).name(), "int"))
						return std::any_cast<int>(b) != 0 ? double(std::any_cast<double>(a) / std::any_cast<int>(b)) : std::any();
					else if (!strcmp(std::type_index(typeid(T2)).name(), "unsigned int"))
						return std::any_cast<unsigned int>(b) != 0 ? double(std::any_cast<double>(a) / std::any_cast<unsigned int>(b)) : std::any();
					else if (!strcmp(std::type_index(typeid(T2)).name(), "double"))
						return std::any_cast<double>(b) != 0 ? double(std::any_cast<double>(a) / std::any_cast<double>(b)) : std::any();
					else if (!strcmp(std::type_index(typeid(T2)).name(), "float"))
						return std::any_cast<float>(b) != 0 ? double(std::any_cast<double>(a) / std::any_cast<float>(b)) : std::any();
					else if (!strcmp(std::type_index(typeid(T2)).name(), "bool"))
						return std::any_cast<bool>(b) != 0 ? double(std::any_cast<double>(a) / std::any_cast<bool>(b)) : std::any();
				}
				else if (!strcmp(std::type_index(typeid(T1)).name(), "float"))
				{
					if (!strcmp(std::type_index(typeid(T2)).name(), "int"))
						return std::any_cast<int>(b) != 0 ? float(std::any_cast<float>(a) / std::any_cast<int>(b)) : std::any();
					else if (!strcmp(std::type_index(typeid(T2)).name(), "unsigned int"))
						return std::any_cast<unsigned int>(b) != 0 ? float(std::any_cast<float>(a) / std::any_cast<unsigned int>(b)) : std::any();
					else if (!strcmp(std::type_index(typeid(T2)).name(), "double"))
						return std::any_cast<double>(b) != 0 ? float(std::any_cast<float>(a) / std::any_cast<double>(b)) : std::any();
					else if (!strcmp(std::type_index(typeid(T2)).name(), "float"))
						return std::any_cast<float>(b) != 0 ? float(std::any_cast<float>(a) / std::any_cast<float>(b)) : std::any();
					else if (!strcmp(std::type_index(typeid(T2)).name(), "bool"))
						return std::any_cast<bool>(b) != 0 ? float(std::any_cast<float>(a) / std::any_cast<bool>(b)) : std::any();
				}
				else if (!strcmp(std::type_index(typeid(T1)).name(), "bool"))
				{
					if (!strcmp(std::type_index(typeid(T2)).name(), "int"))
						return std::any_cast<int>(b) != 0 ? std::any_cast<bool>(a) / std::any_cast<int>(b) : std::any();
					else if (!strcmp(std::type_index(typeid(T2)).name(), "unsigned int"))
						return std::any_cast<unsigned int>(b) != 0 ? std::any_cast<bool>(a) / std::any_cast<unsigned int>(b) : std::any();
					else if (!strcmp(std::type_index(typeid(T2)).name(), "double"))
						return std::any_cast<double>(b) != 0 ? std::any_cast<bool>(a) / std::any_cast<double>(b) : std::any();
					else if (!strcmp(std::type_index(typeid(T2)).name(), "float"))
						return std::any_cast<float>(b) != 0 ? std::any_cast<bool>(a) / std::any_cast<float>(b) : std::any();
					else if (!strcmp(std::type_index(typeid(T2)).name(), "bool"))
						return std::any_cast<bool>(b) != 0 ? std::any_cast<bool>(a) / std::any_cast<bool>(b) : std::any();
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
			}

			return std::any();	//err
		}

		template<typename T1>
		inline std::any AnyOperatorUnary<T1>::DoOp(lex::wcTokenType Op, std::any a)
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
			return std::any();
		}

	}

}


#endif
