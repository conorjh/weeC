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

namespace weec
{
	namespace interpreter
	{
		class wcExpressionInterpeter
		{
			int Value;

			parse::wcParseOutput& Input;
			tree<parse::wcParseNode>::iterator PC;
			parse::wcParseSymbolTable& SymTab;

			std::unordered_map<std::string, std::string> ImplementationTypeNames;
			void SetupImplementationTypeNames();

			std::any ChoosePath(parse::wcParseNodeType Type, parse::wcParseNodeType CalledFrom);

		public:
			wcExpressionInterpeter(parse::wcParseSymbolTable& SymTab, parse::wcParseOutput Input, tree<parse::wcParseNode>::iterator PC);

			std::any ExecSubExpression(),
				ExecEquality(), ExecAssignment(), ExecLogicOr(),
				ExecLogicAnd(), ExecComparison(),
				ExecTerm(), ExecFactor(),
				ExecUnary(), ExecPrimary(),
				ExecOperator();

			std::any Exec();
		};

		class wcInterpreter
		{
			parse::wcParseOutput Input;
			tree<parse::wcParseNode>::iterator PC;
			parse::wcParseSymbolTable SymbolTable;

			bool ExecExpression();

		public:
			wcExpressionInterpeter ExpressionInterp;
			wcInterpreter(parse::wcParseOutput Input);

			void Reset();
			std::any Exec(), ExecStatement();

			bool Halt;
		};

	}

}


#endif
